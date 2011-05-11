// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVar.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/refCounter.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridVar_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridVar_t
gridVar_new(const char *name, gridVarType_t type, int numComponents)
{
	gridVar_t gridVar;

	assert(name != NULL);
	assert(numComponents > 0);

	gridVar                 = xmalloc(sizeof(struct gridVar_struct));
	gridVar->name           = xstrdup(name);
	gridVar->type           = type;
	gridVar->numComponents  = numComponents;
	gridVar->mallocFunc     = NULL;
	gridVar->freeFunc       = NULL;
	gridVar->isFFTWPadded   = false;
	gridVar->isComplexified = false;

	refCounter_init(&(gridVar->refCounter));

	return gridVar_getRef(gridVar);
}

extern gridVar_t
gridVar_clone(const gridVar_t var)
{
	gridVar_t clone;

	assert(var != NULL);

	clone = gridVar_new(var->name,
	                    var->type,
	                    var->numComponents);
	clone->mallocFunc     = var->mallocFunc;
	clone->freeFunc       = var->freeFunc;
	clone->isFFTWPadded   = var->isFFTWPadded;
	clone->isComplexified = var->isComplexified;

	return clone;
}

extern void
gridVar_del(gridVar_t *var)
{
	assert(var != NULL && *var != NULL);

	if (refCounter_deref(&((*var)->refCounter))) {
		xfree((*var)->name);
		xfree(*var);
	}
	*var = NULL;
}

extern gridVar_t
gridVar_getRef(gridVar_t var)
{
	assert(var != NULL);

	refCounter_ref(&(var->refCounter));

	return var;
}

extern size_t
gridVar_getSizePerElement(gridVar_t var)
{
	assert(var != NULL);
	if (var->isComplexified)
		return gridVarType_sizeof(var->type) * 2 * var->numComponents;

	return gridVarType_sizeof(var->type) * var->numComponents;
}

extern int
gridVar_getNumComponents(gridVar_t var)
{
	assert(var != NULL);
	return var->numComponents;
}

extern gridVarType_t
gridVar_getType(gridVar_t var)
{
	assert(var != NULL);
	return var->type;
}

extern char *
gridVar_getName(gridVar_t var)
{
	assert(var != NULL);
	return var->name;
}

extern void
gridVar_setMemFuncs(gridVar_t var,
                    void *(*mallocFunc)(size_t size),
                    void (*freeFunc)(void *ptr))
{
	assert(var != NULL);

	var->mallocFunc = mallocFunc;
	var->freeFunc   = freeFunc;
}

extern void *
gridVar_getMemory(gridVar_t var, uint64_t numElements)
{
	size_t sizeToAlloc;

	assert(var != NULL);
	assert(numElements > UINT64_C(0));

	sizeToAlloc = gridVar_getSizePerElement(var) * numElements;

	if (var->mallocFunc != NULL)
		return var->mallocFunc(sizeToAlloc);

	return xmalloc(sizeToAlloc);
}

extern void *
gridVar_getCopy(gridVar_t var, uint64_t numElements, const void *data)
{
	void *cpy = NULL;

	assert(var != NULL);
	assert(numElements > UINT64_C(0));

	if (data != NULL) {
		size_t totalSize = gridVar_getSizePerElement(var) * numElements;
		cpy = gridVar_getMemory(var, numElements);
		memcpy(cpy, data, totalSize);
	}

	return cpy;
}

extern void
gridVar_freeMemory(gridVar_t var, void *data)
{
	assert(var != NULL);
	assert(data != NULL);

	if (var->freeFunc != NULL)
		var->freeFunc(data);
	else
		xfree(data);
}

extern void *
gridVar_getPointerByOffset(gridVar_t var, const void *base, uint64_t offset)
{
	size_t size;

	assert(var != NULL);
	assert(base != NULL);

	size = gridVar_getSizePerElement(var);

	return (void *)(((const char *)base) + offset * size);
}

extern void
gridVar_setFFTWPadded(gridVar_t var)
{
	assert(var != NULL);
	var->isFFTWPadded = true;
}

extern void
gridVar_unsetFFTWPadded(gridVar_t var)
{
	assert(var != NULL);
	var->isFFTWPadded = false;
}

extern bool
gridVar_isFFTWPadded(gridVar_t var)
{
	assert(var != NULL);
	return var->isFFTWPadded;
}

extern void
gridVar_setComplexified(gridVar_t var)
{
	assert(var != NULL);
	var->isComplexified = true;
}

extern void
gridVar_unsetComplexified(gridVar_t var)
{
	assert(var != NULL);
	var->isComplexified = false;
}

extern bool
gridVar_isComplexified(gridVar_t var)
{
	assert(var != NULL);
	return var->isComplexified;
}

extern void
gridVar_rename(gridVar_t var, const char *newName)
{
	assert(var != NULL);
	assert(newName != NULL && newName[0] != '\0');

	if (var->name != NULL)
		xfree(var->name);
	var->name = xstrdup(newName);
}

#ifdef WITH_MPI
extern MPI_Datatype
gridVar_getMPIDatatype(gridVar_t var)
{
	MPI_Datatype dt = MPI_BYTE;

	assert(var != NULL);

	if (!gridVar_isComplexified(var)) {
		switch (var->type) {
		case GRIDVARTYPE_DOUBLE:
			dt = MPI_DOUBLE;
			break;
		case GRIDVARTYPE_INT:
			dt = MPI_INT;
			break;
		case GRIDVARTYPE_INT8:
			dt = MPI_CHAR;
			break;
		case GRIDVARTYPE_FPV:
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
gridVar_getMPICount(gridVar_t var, uint64_t numElements)
{
	int count;

	assert(var != NULL);

	count = (int)(numElements * gridVar_getSizePerElement(var));

	if (!gridVar_isComplexified(var)) {
		switch (var->type) {
		case GRIDVARTYPE_DOUBLE:
		case GRIDVARTYPE_INT:
		case GRIDVARTYPE_INT8:
		case GRIDVARTYPE_FPV:
			count = (int)numElements;
			break;
		default:
			break;
		}
	}

	return count;
}

#endif

/*--- Implementations of local functions --------------------------------*/
