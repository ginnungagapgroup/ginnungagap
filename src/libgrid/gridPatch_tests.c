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
	for (int i = 0; i < NDIM; i++) {
		if (gridPatch_getOneDim(patch, i) != (idxHi[i] - idxLo[i] + 1))
			hasPassed = false;
	}
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getOneDim_test */

extern bool
gridPatch_getDimActual1D_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       patch;
	gridVar_t         var0, var1;
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
	var0  = gridVar_new("Var0", GRIDVARTYPE_INT, 1);
	var1  = gridVar_new("Var1", GRIDVARTYPE_INT, 1);
	patch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVarData(patch, var0);
	gridPatch_attachVarData(patch, var1);
	for (int i = 0; i < NDIM; i++) {
		uint32_t dimActual   = gridPatch_getDimActual1D(patch, 0, i);
		uint32_t dimExpected = (idxHi[i] - idxLo[i] + 1);
		if (dimActual != dimExpected)
			hasPassed = false;
		dimActual = gridPatch_getDimActual1D(patch, 1, i);
		if (i == NDIM - 1)
			dimExpected = 2 * (dimExpected / 2 + 1);
		if (dimActual != dimExpected)
			hasPassed = false;
	}
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getDimActual1D_test */

extern bool
gridPatch_getDims_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	gridPointUint32_t dims;
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
	gridPatch_getDims(patch, dims);
	for (int i = 0; i < NDIM; i++) {
		if (dims[i] != (idxHi[i] - idxLo[i] + 1))
			hasPassed = false;
	}
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getDims_test */

extern bool
gridPatch_getNumCells_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	uint64_t          numCells       = UINT64_C(1);
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		idxLo[i]  = 123;
		idxHi[i]  = 203;
		numCells *= (idxHi[i] - idxLo[i] + 1);
	}
	patch = gridPatch_new(idxLo, idxHi);
	if (gridPatch_getNumCells(patch) != numCells)
		hasPassed = false;
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getNumCells_test */

extern bool
gridPatch_getNumCellsActual_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	gridVar_t         var0, var1;
	uint64_t          numCellsVar0   = UINT64_C(1);
	uint64_t          numCellsVar1   = UINT64_C(1);
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		idxLo[i]      = 123;
		idxHi[i]      = 203;
		numCellsVar0 *= (idxHi[i] - idxLo[i] + 1);
		if (i < NDIM - 1)
			numCellsVar1 *= (idxHi[i] - idxLo[i] + 1);
		else
			numCellsVar1 *= 2 * ((idxHi[i] - idxLo[i] + 1) / 2 + 1);
	}
	var0  = gridVar_new("Var0", GRIDVARTYPE_INT, 1);
	var1  = gridVar_new("Var1", GRIDVARTYPE_INT, 1);
	patch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVarData(patch, var0);
	gridPatch_attachVarData(patch, var1);
	if (gridPatch_getNumCellsActual(patch, 0) != numCellsVar0)
		hasPassed = false;
	if (gridPatch_getNumCellsActual(patch, 01) != numCellsVar1)
		hasPassed = false;
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getNumCellsActual_test */

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
	for (int i = 0; i < NDIM; i++) {
		if (idxLo2[i] != patch->idxLo[i])
			hasPassed = false;
	}
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getIdxLo_test */

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
	gridPatch = gridPatch_new(idxLo, idxHi);
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
	gridPatch = gridPatch_new(idxLo, idxHi);
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
} /* gridPatch_getVarHandle_test */

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
	gridPatch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVarData(gridPatch, var);
	if (gridPatch_getVarDataHandle(gridPatch, 0)
	    != varArr_getElementHandle(gridPatch->varData, 0))
		hasPassed = false;
	gridPatch_del(&gridPatch);
	gridVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getVarDataHandle_test */

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
	gridPatch = gridPatch_new(idxLo, idxHi);
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
} /* gridPatch_getNumVars_test */

/*--- Implementations of local functions --------------------------------*/
static void
local_fillGrid(const char *param, gridPointUint32_t pos)
{
#if (NDIM == 3)
	return pos[0] * pos[0] + pos[1] * pos[1] + pos[2] * pos[2];

#elif (NDIM == 2)
	return pos[0] * pos[0] + pos[1] * pos[1];
#endif
}
