// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataVarType_tests.h"
#include "dataVarType.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
dataVarType_sizeof_test(void)
{
	bool          hasPassed = true;
	int           rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t        allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (dataVarType_sizeof(DATAVARTYPE_INT) != sizeof(int))
		hasPassed = false;
	if (dataVarType_sizeof(DATAVARTYPE_DOUBLE) != sizeof(double))
		hasPassed = false;
	if (dataVarType_sizeof(DATAVARTYPE_FPV) != sizeof(fpv_t))
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVarType_isFloating_test(void)
{
	bool          hasPassed = true;
	int           rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t        allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (dataVarType_isFloating(DATAVARTYPE_INT))
		hasPassed = false;
	if (!dataVarType_isFloating(DATAVARTYPE_DOUBLE))
		hasPassed = false;
	if (!dataVarType_isFloating(DATAVARTYPE_FPV))
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVarType_isInteger_test(void)
{
	bool          hasPassed = true;
	int           rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t        allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (!dataVarType_isInteger(DATAVARTYPE_INT))
		hasPassed = false;
	if (dataVarType_isInteger(DATAVARTYPE_DOUBLE))
		hasPassed = false;
	if (dataVarType_isInteger(DATAVARTYPE_FPV))
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVarType_isNativeFloat_test(void)
{
	bool          hasPassed = true;
	int           rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t        allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (dataVarType_isNativeFloat(DATAVARTYPE_INT))
		hasPassed = false;
	if (dataVarType_isNativeFloat(DATAVARTYPE_DOUBLE))
		hasPassed = false;
#ifdef ENABLE_DOUBLE
	if (dataVarType_isNativeFloat(DATAVARTYPE_FPV))
		hasPassed = false;
#else
	if (!dataVarType_isNativeFloat(DATAVARTYPE_FPV))
		hasPassed = false;
#endif
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataVarType_isNativeDouble_test(void)
{
	bool          hasPassed = true;
	int           rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t        allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (dataVarType_isNativeDouble(DATAVARTYPE_INT))
		hasPassed = false;
	if (!dataVarType_isNativeDouble(DATAVARTYPE_DOUBLE))
		hasPassed = false;
#ifdef ENABLE_DOUBLE
	if (!dataVarType_isNativeDouble(DATAVARTYPE_FPV))
		hasPassed = false;
#else
	if (dataVarType_isNativeDouble(DATAVARTYPE_FPV))
		hasPassed = false;
#endif
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
