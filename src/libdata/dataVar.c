// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataVar.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_HDF5
#  include <hdf5.h>
#endif
#include "../libutil/refCounter.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "dataVar_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern dataVar_t
dataVar_new(const char *name, dataVarType_t type, int numComponents)
{
	dataVar_t dataVar;

	assert(name != NULL);
	assert(numComponents > 0);

	dataVar                 = xmalloc(sizeof(struct dataVar_struct));
	dataVar->name           = xstrdup(name);
	dataVar->type           = type;
	dataVar->numComponents  = numComponents;
	dataVar->mallocFunc     = NULL;
	dataVar->freeFunc       = NULL;
	dataVar->isFFTWPadded   = false;
	dataVar->isComplexified = false;

	refCounter_init(&(dataVar->refCounter));

	return dataVar_getRef(dataVar);
}

extern dataVar_t
dataVar_clone(const dataVar_t var)
{
	dataVar_t clone;

	assert(var != NULL);

	clone = dataVar_new(var->name,
	                    var->type,
	                    var->numComponents);
	clone->mallocFunc     = var->mallocFunc;
	clone->freeFunc       = var->freeFunc;
	clone->isFFTWPadded   = var->isFFTWPadded;
	clone->isComplexified = var->isComplexified;

	return clone;
}

extern void
dataVar_del(dataVar_t *var)
{
	assert(var != NULL && *var != NULL);

	if (refCounter_deref(&((*var)->refCounter))) {
		xfree((*var)->name);
		xfree(*var);
	}
	*var = NULL;
}

extern dataVar_t
dataVar_getRef(dataVar_t var)
{
	assert(var != NULL);

	refCounter_ref(&(var->refCounter));

	return var;
}

extern size_t
dataVar_getSizePerElement(dataVar_t var)
{
	assert(var != NULL);
	if (var->isComplexified)
		return dataVarType_sizeof(var->type) * 2 * var->numComponents;

	return dataVarType_sizeof(var->type) * var->numComponents;
}

extern int
dataVar_getNumComponents(dataVar_t var)
{
	assert(var != NULL);
	return var->numComponents;
}

extern dataVarType_t
dataVar_getType(dataVar_t var)
{
	assert(var != NULL);
	return var->type;
}

extern char *
dataVar_getName(dataVar_t var)
{
	assert(var != NULL);
	return var->name;
}

extern void
dataVar_setMemFuncs(dataVar_t var,
                    void *(*mallocFunc)(size_t size),
                    void (*freeFunc)(void *ptr))
{
	assert(var != NULL);

	var->mallocFunc = mallocFunc;
	var->freeFunc   = freeFunc;
}

extern void *
dataVar_getMemory(dataVar_t var, uint64_t numElements)
{
	size_t sizeToAlloc;

	assert(var != NULL);
	assert(numElements > UINT64_C(0));

	sizeToAlloc = dataVar_getSizePerElement(var) * numElements;

	if (var->mallocFunc != NULL)
		return var->mallocFunc(sizeToAlloc);

	return xmalloc(sizeToAlloc);
}

extern void *
dataVar_getCopy(dataVar_t var, uint64_t numElements, const void *data)
{
	void *cpy = NULL;

	assert(var != NULL);
	assert(numElements > UINT64_C(0));

	if (data != NULL) {
		size_t totalSize = dataVar_getSizePerElement(var) * numElements;
		cpy = dataVar_getMemory(var, numElements);
		memcpy(cpy, data, totalSize);
	}

	return cpy;
}

extern void
dataVar_freeMemory(dataVar_t var, void *data)
{
	assert(var != NULL);
	assert(data != NULL);

	if (var->freeFunc != NULL)
		var->freeFunc(data);
	else
		xfree(data);
}

extern void *
dataVar_getPointerByOffset(dataVar_t var, const void *base, uint64_t offset)
{
	size_t size;

	assert(var != NULL);
	assert(base != NULL);

	size = dataVar_getSizePerElement(var);

	return (void *)(((const char *)base) + offset * size);
}

extern void
dataVar_setFFTWPadded(dataVar_t var)
{
	assert(var != NULL);
	var->isFFTWPadded = true;
}

extern void
dataVar_unsetFFTWPadded(dataVar_t var)
{
	assert(var != NULL);
	var->isFFTWPadded = false;
}

extern bool
dataVar_isFFTWPadded(dataVar_t var)
{
	assert(var != NULL);
	return var->isFFTWPadded;
}

extern void
dataVar_setComplexified(dataVar_t var)
{
	assert(var != NULL);
	var->isComplexified = true;
}

extern void
dataVar_unsetComplexified(dataVar_t var)
{
	assert(var != NULL);
	var->isComplexified = false;
}

extern bool
dataVar_isComplexified(dataVar_t var)
{
	assert(var != NULL);
	return var->isComplexified;
}

extern void
dataVar_rename(dataVar_t var, const char *newName)
{
	assert(var != NULL);
	assert(newName != NULL && newName[0] != '\0');

	if (var->name != NULL)
		xfree(var->name);
	var->name = xstrdup(newName);
}

#ifdef WITH_MPI
extern MPI_Datatype
dataVar_getMPIDatatype(dataVar_t var)
{
	MPI_Datatype dt = MPI_BYTE;

	assert(var != NULL);

	if (!dataVar_isComplexified(var)) {
		switch (var->type) {
		case DATAVARTYPE_DOUBLE:
			dt = MPI_DOUBLE;
			break;
		case DATAVARTYPE_INT:
			dt = MPI_INT;
			break;
		case DATAVARTYPE_INT8:
			dt = MPI_CHAR;
			break;
		case DATAVARTYPE_FPV:
#  ifdef ENABLE_DOUBLE
			dt = MPI_DOUBLE;
#  else
			dt = MPI_FLOAT;
#  endif
			break;
		default:
			break;
		}
	}

	return dt;
}

extern int
dataVar_getMPICount(dataVar_t var, uint64_t numElements)
{
	int count;

	assert(var != NULL);

	count = (int)(numElements * dataVar_getSizePerElement(var));

	if (!dataVar_isComplexified(var)) {
		switch (var->type) {
		case DATAVARTYPE_DOUBLE:
		case DATAVARTYPE_INT:
		case DATAVARTYPE_INT8:
		case DATAVARTYPE_FPV:
			count = (int)numElements;
			break;
		default:
			break;
		}
	}

	return count;
}

#endif

#ifdef WITH_HDF5
extern hid_t
dataVar_getHDF5Datatype(const dataVar_t var)
{
	assert(var != NULL);

	hid_t dt, baseDt;

	switch (var->type) {
	case DATAVARTYPE_DOUBLE:
		baseDt = H5T_NATIVE_DOUBLE;
		break;
	case DATAVARTYPE_INT:
		baseDt = H5T_NATIVE_INT;
		break;
	case DATAVARTYPE_INT8:
		baseDt = H5T_NATIVE_CHAR;
		break;
	case DATAVARTYPE_FPV:
#  ifdef ENABLE_DOUBLE
		baseDt = H5T_NATIVE_DOUBLE;
#  else
		baseDt = H5T_NATIVE_FLOAT;
#  endif
		break;
	}

	if (var->numComponents == 1) {
		dt = H5Tcopy(baseDt);
	} else {
		hsize_t tmp[1] = { var->numComponents };
		dt = H5Tarray_create(baseDt, 1, tmp);
	}

	return dt;
} /* dataVar_getHDF5Datatype */

#endif

/*--- Implementations of local functions --------------------------------*/
