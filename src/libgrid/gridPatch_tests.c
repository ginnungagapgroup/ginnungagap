// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch_tests.h"
#include "gridPatch.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridPatch_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
#if (NDIM == 2)
static bool
local_tranposeVar_test_2d(void);


#elif (NDIM == 3)
static bool
local_tranposeVar_test_3d(void);

#endif

static gridPatch_t
local_getFakePatch(void);

static bool
local_verifyFakePatchTransposed(gridPatch_t patch, gridPointInt_t s);

static gridPatch_t
local_getFakePatchForCopy(void);


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
	dataVar_t         var0, var1;
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
		idxHi[i] = 16;
	}
	var0  = dataVar_new("Var0", DATAVARTYPE_INT, 1);
	var1  = dataVar_new("Var1", DATAVARTYPE_INT, 1);
	patch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(patch, var0);
	gridPatch_attachVar(patch, var1);
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
gridPatch_getDimsActual_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	gridPointUint32_t dims;
	dataVar_t var;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 12;
		idxHi[i] = 16;
	}
	patch = gridPatch_new(idxLo, idxHi);
	var = dataVar_new("basd", DATAVARTYPE_INT, 1);
	dataVar_setFFTWPadded(var);
	gridPatch_attachVar(patch, var);
	gridPatch_getDimsActual(patch, 0, dims);
	if (dims[0] != (2 * ((idxHi[0] - idxLo[0] + 1)/2 + 1)))
		hasPassed = false;
	for (int i = 1; i < NDIM; i++) {
		if (dims[i] != (idxHi[i] - idxLo[i] + 1))
			hasPassed = false;
	}
	dataVar_del(&var);
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

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
	dataVar_t         var0, var1;
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
		idxHi[i]      = 125;
		numCellsVar0 *= (idxHi[i] - idxLo[i] + 1);
		if (i < NDIM - 1)
			numCellsVar1 *= (idxHi[i] - idxLo[i] + 1);
		else
			numCellsVar1 *= 2 * ((idxHi[i] - idxLo[i] + 1) / 2 + 1);
	}
	var0  = dataVar_new("Var0", DATAVARTYPE_INT, 1);
	var1  = dataVar_new("Var1", DATAVARTYPE_INT, 1);
	patch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(patch, var0);
	gridPatch_attachVar(patch, var1);
	if (gridPatch_getNumCellsActual(patch, 0) != numCellsVar0)
		hasPassed = false;
	if (gridPatch_getNumCellsActual(patch, 1) != numCellsVar1)
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
gridPatch_attachVar_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	dataVar_t         var;
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

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 12;
		idxHi[i] = 13;
	}
	gridPatch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(gridPatch, var);
	gridPatch_del(&gridPatch);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_attachVar_test */

extern bool
gridPatch_detachVar_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	dataVar_t         var, varTmp;
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

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 1;
	}
	gridPatch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(gridPatch, var);
	gridPatch_attachVar(gridPatch, var);
	gridPatch_attachVar(gridPatch, var);
	gridPatch_attachVar(gridPatch, var);
	varTmp = gridPatch_detachVar(gridPatch, 3);
	dataVar_del(&varTmp);
	varTmp = gridPatch_detachVar(gridPatch, 2);
	dataVar_del(&varTmp);
	varTmp = gridPatch_detachVar(gridPatch, 1);
	dataVar_del(&varTmp);
	varTmp = gridPatch_detachVar(gridPatch, 0);
	dataVar_del(&varTmp);
	gridPatch_del(&gridPatch);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_detachVar_test */

extern bool
gridPatch_allocateVarData_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	dataVar_t         var;
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

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, 1);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 1;
	}
	gridPatch   = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(gridPatch, var);

	if (varArr_getElementHandle(gridPatch->varData, 0) != NULL)
		hasPassed = false;
	gridPatch_allocateVarData(gridPatch, 0);
	if (varArr_getElementHandle(gridPatch->varData, 0) == NULL)
		hasPassed = false;

	gridPatch_del(&gridPatch);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_freeVarData_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	dataVar_t         var;
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

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, 1);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 1;
	}
	gridPatch   = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(gridPatch, var);

	if (varArr_getElementHandle(gridPatch->varData, 0) != NULL)
		hasPassed = false;
	gridPatch_allocateVarData(gridPatch, 0);
	if (varArr_getElementHandle(gridPatch->varData, 0) == NULL)
		hasPassed = false;
	gridPatch_freeVarData(gridPatch, 0);
	if (varArr_getElementHandle(gridPatch->varData, 0) != NULL)
		hasPassed = false;

	gridPatch_del(&gridPatch);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_replaceVarData_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	dataVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	void              *replaceData;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, 1);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 1;
	}
	gridPatch   = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(gridPatch, var);
	replaceData = dataVar_getMemory(var, gridPatch->numCells);
	gridPatch_replaceVarData(gridPatch, 0, replaceData);
	if (gridPatch_getVarDataHandle(gridPatch, 0) != replaceData)
		hasPassed = false;
	gridPatch_del(&gridPatch);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_replaceVarData_test */

extern bool
gridPatch_getVarHandle_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       gridPatch;
	dataVar_t         var;
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

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 1;
	}
	gridPatch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(gridPatch, var);
	if (gridPatch_getVarHandle(gridPatch, 0) != var)
		hasPassed = false;
	gridPatch_del(&gridPatch);
	dataVar_del(&var);
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
	dataVar_t         var;
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

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 1;
	}
	gridPatch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(gridPatch, var);
	if (gridPatch_getVarDataHandle(gridPatch, 0)
	    != varArr_getElementHandle(gridPatch->varData, 0))
		hasPassed = false;
	gridPatch_del(&gridPatch);
	dataVar_del(&var);
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
	dataVar_t         var;
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

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, NDIM);
	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = 1;
	}
	gridPatch = gridPatch_new(idxLo, idxHi);
	if (gridPatch_getNumVars(gridPatch) != 0)
		hasPassed = false;
	gridPatch_attachVar(gridPatch, var);
	if (gridPatch_getNumVars(gridPatch) != 1)
		hasPassed = false;
	gridPatch_del(&gridPatch);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridPatch_getNumVars_test */

extern bool
gridPatch_transpose_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

#if (NDIM == 2)
	if (!local_tranposeVar_test_2d())
		hasPassed = false;
#elif (NDIM == 3)
	if (!local_tranposeVar_test_3d())
		hasPassed = false;
#endif
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_getWindowedDataCopy_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	uint64_t          numElements;
	int               *data;
	int               offset         = 0;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	patch = local_getFakePatchForCopy();

	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 3;
		idxHi[i] = 4;
	}
	data = gridPatch_getWindowedDataCopy(patch, 0, idxLo, idxHi,
	                                     &numElements);
#if (NDIM == 2)
	if (numElements != 4)
		hasPassed = false;
	for (int j = 3; j <= 4; j++) {
		for (int i = 3; i <= 4; i++) {
			int expected = i + j * (patch->idxLo[0] + patch->dims[0]);
			if (data[offset++] != expected)
				hasPassed = false;
		}
	}
#elif (NDIM == 3)
	if (numElements != 8)
		hasPassed = false;
	for (int k = 3; k <= 4; k++) {
		for (int j = 3; j <= 4; j++) {
			for (int i = 3; i <= 4; i++) {
				int expected = i + j * (patch->idxLo[0] + patch->dims[0])
				               + k * (patch->idxLo[0] + patch->dims[0])
				               * (patch->idxLo[1] + patch->dims[1]);
				if (data[offset++] != expected)
					hasPassed = false;
			}
		}
	}
#endif
	xfree(data);

	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridPatch_putWindowedData_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	dataVar_t var;
	gridPatch_t       patch;
	gridPointUint32_t idxLoPatch, idxHiPatch;
	gridPointUint32_t idxLoWindow, idxHiWindow;
	double            *data, *dataWindow, *dataWindowCopy;
	uint64_t numCellsPatch, numCellsWindow;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = dataVar_new("TEST", DATAVARTYPE_DOUBLE, 1);

	for (int i = 0; i < NDIM; i++) {
		idxLoPatch[i] = 0;
		idxHiPatch[i] = 15;
	}
	patch = gridPatch_new(idxLoPatch, idxHiPatch);
	gridPatch_attachVar(patch, var);
	data = (double *)gridPatch_getVarDataHandle(patch, 0);
	numCellsPatch = gridPatch_getNumCells(patch);
	for (uint64_t i=0; i<numCellsPatch; i++)
		data[i] = 123.0;

	numCellsWindow=1;
	for (int i = 0; i < NDIM; i++) {
		idxLoWindow[i] = 3;
		idxHiWindow[i] = 5;
		numCellsWindow *= 5-3+1;
	}
	dataWindow = dataVar_getMemory(var, numCellsWindow);
	for (uint64_t i=0; i<numCellsWindow; i++)
		dataWindow[i] = -12.0;

	gridPatch_putWindowedData(patch, 0, idxLoWindow, idxHiWindow,
	                          dataWindow);

	dataWindowCopy = gridPatch_getWindowedDataCopy(patch, 0, idxLoWindow,
	                                               idxHiWindow, NULL);

	for (uint64_t i=0; i<numCellsWindow; i++) {
		if (islessgreater(dataWindow[i], dataWindowCopy[i]))
			hasPassed = false;
	}

	dataVar_freeMemory(var, dataWindowCopy);
	dataVar_freeMemory(var, dataWindow);
	gridPatch_del(&patch);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
#if (NDIM == 2)
static bool
local_tranposeVar_test_2d(void)
{
	bool           hasPassed = true;
	gridPatch_t    patch;
	gridPointInt_t s;

	patch = local_getFakePatch();

	gridPatch_transpose(patch, 0, 1);
	s[0] = 1;
	s[1] = 0;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;

	gridPatch_transpose(patch, 0, 1);
	s[0] = 0;
	s[1] = 1;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;

	gridPatch_del(&patch);

	return hasPassed ? true : false;
}

#elif (NDIM == 3)
static bool
local_tranposeVar_test_3d(void)
{
	bool           hasPassed = true;
	gridPatch_t    patch;
	gridPointInt_t s;

	patch = local_getFakePatch();

	gridPatch_transpose(patch, 0, 1);
	s[0] = 1;
	s[1] = 0;
	s[2] = 2;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;
	gridPatch_transpose(patch, 0, 1);
	s[0] = 0;
	s[1] = 1;
	s[2] = 2;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;

	gridPatch_transpose(patch, 0, 2);
	s[0] = 2;
	s[1] = 1;
	s[2] = 0;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;
	gridPatch_transpose(patch, 0, 2);
	s[0] = 0;
	s[1] = 1;
	s[2] = 2;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;

	gridPatch_transpose(patch, 1, 2);
	s[0] = 0;
	s[1] = 2;
	s[2] = 1;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;
	gridPatch_transpose(patch, 1, 2);
	s[0] = 0;
	s[1] = 1;
	s[2] = 2;
	if (!local_verifyFakePatchTransposed(patch, s))
		hasPassed = false;

	gridPatch_del(&patch);

	return hasPassed ? true : false;
} /* local_tranposeVar_test_3d */

#endif

static gridPatch_t
local_getFakePatch(void)
{
	gridPatch_t       patch;
	dataVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	int               *data;
	int               offset = 0;

	var      = dataVar_new("TEST", DATAVARTYPE_INT, 1);
	idxLo[0] = 0;
#ifdef WITH_MPI
	idxHi[0] = 32;
#else
	idxHi[0] = 150;
#endif
	idxLo[1] = 0;
#ifdef WITH_MPI
	idxHi[1] = 33;
#else
	idxHi[1] = 150;
#endif
#if (NDIM > 2)
	idxLo[2] = 0;
#  ifdef WITH_MPI
	idxHi[2] = 34;
#  else
	idxHi[2] = 150;
#  endif
#endif
	patch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(patch, var);
	data  = gridPatch_getVarDataHandle(patch, 0);
#if (NDIM == 2)
	for (int j = 0; j < patch->dims[1]; j++) {
		for (int i = 0; i < patch->dims[0]; i++) {
			data[offset] = offset;
			offset++;
		}
	}
#elif (NDIM == 3)
	for (int k = 0; k < patch->dims[2]; k++) {
		for (int j = 0; j < patch->dims[1]; j++) {
			for (int i = 0; i < patch->dims[0]; i++) {
				data[offset] = offset;
				offset++;
			}
		}
	}
#endif
	dataVar_del(&var);

	return patch;
} /* local_getFakePatch */

static bool
local_verifyFakePatchTransposed(gridPatch_t patch, gridPointInt_t s)
{
	int            *data;
	gridPointInt_t k;
	int            expected;
	int            offset = 0;

	data = gridPatch_getVarDataHandle(patch, 0);

#if (NDIM == 2)
	for (k[1] = 0; k[1] < patch->dims[1]; k[1]++) {
		for (k[0] = 0; k[0] < patch->dims[0]; k[0]++) {
			expected = k[s[0]] + k[s[1]] * patch->dims[s[0]];
			if (data[offset] != expected)
				return false;

			offset++;
		}
	}
#elif (NDIM == 3)
	for (k[2] = 0; k[2] < patch->dims[2]; k[2]++) {
		for (k[1] = 0; k[1] < patch->dims[1]; k[1]++) {
			for (k[0] = 0; k[0] < patch->dims[0]; k[0]++) {
				expected = k[s[0]] + k[s[1]] * patch->dims[s[0]]
				           + k[s[2]] * patch->dims[s[0]]
				           * patch->dims[s[1]];
				if (data[offset] != expected)
					return false;

				offset++;
			}
		}
	}
#endif

	return true;
} /* local_verifyFakePatchTransposed */

static gridPatch_t
local_getFakePatchForCopy(void)
{
	gridPatch_t       patch;
	dataVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	int               *data;
	int               offset = 0;

	var      = dataVar_new("TEST", DATAVARTYPE_INT, 1);
	idxLo[0] = 1;
	idxHi[0] = 32;
	idxLo[1] = 2;
	idxHi[1] = 33;
#if (NDIM > 2)
	idxLo[2] = 3;
	idxHi[2] = 34;
#endif
	patch    = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(patch, var);
	data     = gridPatch_getVarDataHandle(patch, 0);
#if (NDIM == 2)
	for (int j = idxLo[1]; j <= idxHi[1]; j++) {
		for (int i = idxLo[0]; i <= idxHi[0]; i++) {
			data[offset] = i + j * (idxHi[0] + 1);
			offset++;
		}
	}
#elif (NDIM == 3)
	for (int k = idxLo[2]; k <= idxHi[2]; k++) {
		for (int j = idxLo[1]; j <= idxHi[1]; j++) {
			for (int i = idxLo[0]; i <= idxHi[0]; i++) {
				data[offset] = i + j * (idxHi[0] + 1)
				               + k * (idxHi[0] + 1) * (idxHi[1] + 1);
				offset++;
			}
		}
	}
#endif
	dataVar_del(&var);

	return patch;
} /* local_getFakePatch */
