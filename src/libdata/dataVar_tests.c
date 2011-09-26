// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataVar_tests.h"
#include "dataVar.h"
#include "dataVarType.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_HDF5
#  include <hdf5.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "dataVar_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_TESTNAME "ad12dklkg3t13t"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
dataVar_new_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	if (strcmp(dataVar->name, LOCAL_TESTNAME) != 0)
		hasPassed = false;
	if (dataVar->type != DATAVARTYPE_DOUBLE)
		hasPassed = false;
	if (dataVar->numComponents != NDIM)
		hasPassed = false;
	if (dataVar->isFFTWPadded)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_clone_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar, clone;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	clone   = dataVar_clone(dataVar);
	if (strcmp(dataVar->name, clone->name) != 0)
		hasPassed = false;
	if (dataVar->type != clone->type)
		hasPassed = false;
	if (dataVar->numComponents != clone->numComponents)
		hasPassed = false;
	if (dataVar->isFFTWPadded != clone->isFFTWPadded)
		hasPassed = false;
	dataVar_del(&dataVar);
	dataVar_del(&clone);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* dataVar_clone_test */

extern bool
dataVar_del_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	dataVar_del(&dataVar);
	if (dataVar != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_getRef_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t var, varBackup;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var       = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	varBackup = dataVar_getRef(var);
	if (varBackup->name != var->name)
		hasPassed = false;
	if (varBackup->refCounter != var->refCounter)
		hasPassed = false;
	if (varBackup->refCounter != 2)
		hasPassed = false;
	dataVar_del(&var);
	if (varBackup->refCounter != 1)
		hasPassed = false;
	dataVar_del(&varBackup);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* dataVar_getRef_test */

extern bool
dataVar_getSizePerElement_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	if (dataVar_getSizePerElement(dataVar) != sizeof(double) * NDIM)
		hasPassed = false;
	dataVar_setComplexified(dataVar);
	if (dataVar_getSizePerElement(dataVar) != sizeof(double) * NDIM * 2)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_getNumComponents_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	if (dataVar_getNumComponents(dataVar) != NDIM)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_getType_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	if (dataVar_getType(dataVar) != DATAVARTYPE_DOUBLE)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_getName_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	if (strcmp(dataVar_getName(dataVar), LOCAL_TESTNAME) != 0)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_setMemFuncs_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	dataVar_setMemFuncs(dataVar, NULL, NULL);
	if ((dataVar->mallocFunc != NULL) || (dataVar->freeFunc != NULL))
		hasPassed = false;
	dataVar_setMemFuncs(dataVar, &malloc, &free);
	if ((dataVar->mallocFunc != &malloc) || (dataVar->freeFunc != &free))
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_getMemory_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	void      *tmp;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	tmp     = dataVar_getMemory(dataVar, UINT64_C(1024));
	if (tmp == NULL)
		hasPassed = false;
	dataVar_freeMemory(dataVar, tmp);
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_freeMemory_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	void      *tmp;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	tmp     = dataVar_getMemory(dataVar, UINT64_C(1024));
	dataVar_freeMemory(dataVar, tmp);
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_getPointerByOffset_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	void      *tmp;
	double    *tmpCorrect;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar    = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, 1);
	tmp        = dataVar_getMemory(dataVar, UINT64_C(1024));
	tmpCorrect = (double *)tmp;

	if (dataVar_getPointerByOffset(dataVar, tmp, 14) != tmpCorrect + 14)
		hasPassed = false;
	if (dataVar_getPointerByOffset(dataVar, tmp, 321) != tmpCorrect + 321)
		hasPassed = false;
	if (dataVar_getPointerByOffset(dataVar, tmp, 0) != tmpCorrect)
		hasPassed = false;

	dataVar_freeMemory(dataVar, tmp);
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* dataVar_getPointerByOffset_test */

extern bool
dataVar_setFFTWPadded_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	dataVar_setFFTWPadded(dataVar);
	if (!dataVar->isFFTWPadded)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_unsetFFTWPadded_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	dataVar_setFFTWPadded(dataVar);
	dataVar_unsetFFTWPadded(dataVar);
	if (dataVar->isFFTWPadded)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_isFFTWPadded_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	if (dataVar_isFFTWPadded(dataVar))
		hasPassed = false;
	dataVar_setFFTWPadded(dataVar);
	if (!dataVar_isFFTWPadded(dataVar))
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_setComplexified_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	dataVar_setComplexified(dataVar);
	if (!dataVar->isComplexified)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_unsetComplexified_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	dataVar_unsetComplexified(dataVar);
	if (dataVar->isComplexified)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVar_isComplexified_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, NDIM);
	if (dataVar->isComplexified)
		hasPassed = false;
	dataVar_setComplexified(dataVar);
	if (!dataVar->isComplexified)
		hasPassed = false;
	dataVar_unsetComplexified(dataVar);
	if (dataVar->isComplexified)
		hasPassed = false;
	dataVar_del(&dataVar);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

#ifdef WITH_MPI
extern bool
dataVar_getMPIDatatype_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#  ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#  endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, 1);
	if (dataVar_getMPIDatatype(dataVar) != MPI_DOUBLE)
		hasPassed = false;
	dataVar_setComplexified(dataVar);
	if (dataVar_getMPIDatatype(dataVar) != MPI_BYTE)
		hasPassed = false;
	dataVar_del(&dataVar);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

extern bool
dataVar_getMPICount_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
#  ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#  endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, 1);
	if (dataVar_getMPICount(dataVar, 2) != 2)
		hasPassed = false;
	dataVar_setComplexified(dataVar);
	if (dataVar_getMPICount(dataVar, 2) != 2 * 2 * sizeof(double))
		hasPassed = false;
	dataVar_del(&dataVar);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

#endif

#ifdef WITH_HDF5
extern bool
dataVar_getHDF5Datatype_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	dataVar_t dataVar;
	hid_t     dt, dtBase;
	hsize_t   dims[1];
#  ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#  endif
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, 1);
	dt      = dataVar_getHDF5Datatype(dataVar);
	if (!H5Tequal(dt, H5T_NATIVE_DOUBLE))
		hasPassed = false;
	dataVar_del(&dataVar);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_INT, 1);
	dt      = dataVar_getHDF5Datatype(dataVar);
	if (!H5Tequal(dt, H5T_NATIVE_INT))
		hasPassed = false;
	dataVar_del(&dataVar);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_INT8, 1);
	dt      = dataVar_getHDF5Datatype(dataVar);
	if (!H5Tequal(dt, H5T_NATIVE_CHAR))
		hasPassed = false;
	dataVar_del(&dataVar);

	dataVar = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_DOUBLE, 3);
	dt      = dataVar_getHDF5Datatype(dataVar);
	if (H5Tget_class(dt) != H5T_ARRAY)
		hasPassed = false;
	dtBase = H5Tget_super(dt);
	if (!H5Tequal(dtBase, H5T_NATIVE_DOUBLE))
		hasPassed = false;
	if (H5Tget_array_ndims(dt) != 1)
		hasPassed = false;
	H5Tget_array_dims(dt, dims);
	if (dims[0] != 3)
		hasPassed = false;
	dataVar_del(&dataVar);

#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
} /* dataVar_getHDF5Datatype_test */
#endif

/*--- Implementations of local functions --------------------------------*/
