// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch_tests.h"
#include "gridPatch.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridPatch_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridPatch_new_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 12;
		idxHi[i] = 231;
	}
	idxHi[NDIM - 1] = idxLo[NDIM - 1];
	gridPatch       = gridPatch_new(idxLo, idxHi);
	for (int i = 0; i < NDIM; i++) {
		if (gridPatch->idxLo[i] != idxLo[i])
			hasPassed = false;
		if (gridPatch->dims[i] != idxHi[i] - idxLo[i] + 1)
			hasPassed = false;
	}
	gridPatch_del(&gridPatch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_new_test */

extern bool
gridPatch_del_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 12;
		idxHi[i] = 231;
	}
	gridPatch = gridPatch_new(idxLo, idxHi);
	gridPatch_del(&gridPatch);
	if (gridPatch != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_getOneDim_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 12;
		idxHi[i] = 231;
	}
	patch = gridPatch_new(idxLo, idxHi);
	for (int i=0; i<NDIM; i++) {
		if (gridPatch_getOneDim(patch, i) != (idxHi[i]-idxLo[i]+1))
			hasPassed = false;
	}
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_getIdxLo_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxLo2;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 12;
		idxHi[i] = 231;
	}
	patch = gridPatch_new(idxLo, idxHi);
	gridPatch_getIdxLo(patch, idxLo2);
	for (int i=0; i<NDIM; i++) {
		if (idxLo2[i] != patch->idxLo[i])
			hasPassed = false;
	}
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_attachVarData_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	gridVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new("TEST", GRIDVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 12;
		idxHi[i] = 47;
	}
	gridPatch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVarData(gridPatch, var);
	gridPatch_del(&gridPatch);
	gridVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_attachVarData_test */

extern bool
gridPatch_detachVarData_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	gridVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new("TEST", GRIDVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 31;
	}
	gridPatch    = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVarData(gridPatch, var);
	gridPatch_attachVarData(gridPatch, var);
	gridPatch_attachVarData(gridPatch, var);
	gridPatch_attachVarData(gridPatch, var);
	xfree(gridPatch_detachVarData(gridPatch, 3));
	xfree(gridPatch_detachVarData(gridPatch, 2));
	xfree(gridPatch_detachVarData(gridPatch, 1));
	xfree(gridPatch_detachVarData(gridPatch, 0));
	gridPatch_del(&gridPatch);
	gridVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_detachVarData_test */

extern bool
gridPatch_getVarHandle_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	gridVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new("TEST", GRIDVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 31;
	}
	gridPatch    = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVarData(gridPatch, var);
	if (gridPatch_getVarHandle(gridPatch, 0) != var)
		hasPassed = false;
	gridPatch_del(&gridPatch);
	gridVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_getVarDataHandle_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	gridVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new("TEST", GRIDVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 31;
	}
	gridPatch    = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVarData(gridPatch, var);
	if (   gridPatch_getVarDataHandle(gridPatch, 0)
	    != varArr_getElementHandle(gridPatch->varData, 0))
		hasPassed = false;
	gridPatch_del(&gridPatch);
	gridVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}


extern bool
gridPatch_getNumVars_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	gridVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new("TEST", GRIDVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 31;
	}
	gridPatch    = gridPatch_new(idxLo, idxHi);
	if (gridPatch_getNumVars(gridPatch) != 0)
		hasPassed = false;
	gridPatch_attachVarData(gridPatch, var);
	if (gridPatch_getNumVars(gridPatch) != 1)
		hasPassed = false;
	gridPatch_del(&gridPatch);
	gridVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
