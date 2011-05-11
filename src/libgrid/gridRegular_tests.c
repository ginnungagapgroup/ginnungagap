// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular_tests.h"
#include "gridRegular.h"
#include "../libdata/dataVar.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegular_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_TESTNAME "muksligukkel"


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getFakeGrid(gridPointDbl_t    origin,
                  gridPointDbl_t    extent,
                  gridPointUint32_t dims);

static gridPatch_t
local_getFakePatch(void);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridRegular_new_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	if (strcmp(LOCAL_TESTNAME, grid->name) != 0)
		hasPassed = false;
	for (int i = 0; i < NDIM; i++) {
		if (islessgreater(grid->origin[i], origin[i]))
			hasPassed = false;
		if (islessgreater(grid->extent[i], extent[i]))
			hasPassed = false;
		if (grid->dims[i] != dims[i])
			hasPassed = false;
	}
	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_new_test */

extern bool
gridRegular_del_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_del(&grid);
	if (grid != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_del_test */

extern bool
gridRegular_getRef_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid, gridBackup;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid       = local_getFakeGrid(origin, extent, dims);
	gridBackup = gridRegular_getRef(grid);
	if (gridBackup->name != grid->name)
		hasPassed = false;
	if (gridBackup->refCounter != grid->refCounter)
		hasPassed = false;
	if (gridBackup->refCounter != 2)
		hasPassed = false;
	gridRegular_del(&grid);
	if (gridBackup->refCounter != 1)
		hasPassed = false;
	gridRegular_del(&gridBackup);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getRef_test */

extern bool
gridRegular_getName_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	if (strcmp(gridRegular_getName(grid), LOCAL_TESTNAME) != 0)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegular_getOrigin_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    origin2;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getOrigin(grid, origin2);
	for (int i = 0; i < NDIM; i++) {
		if (islessgreater(origin2[i], grid->origin[i]))
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getOrigin_test */

extern bool
gridRegular_getExtent_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointDbl_t    extent2;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getExtent(grid, extent2);
	for (int i = 0; i < NDIM; i++) {
		if (islessgreater(extent2[i], grid->extent[i]))
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getExtent_test */

extern bool
gridRegular_getDelta_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointDbl_t    delta;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getDelta(grid, delta);
	for (int i = 0; i < NDIM; i++) {
		if (islessgreater(delta[i], grid->delta[i]))
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getDelta_test */

extern bool
gridRegular_getDims_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPointUint32_t dimsTest;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getDims(grid, dimsTest);
	for (int i = 0; i < NDIM; i++) {
		if (dims[i] != dimsTest[i])
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getDims_test */

extern bool
gridRegular_getDimsComplex_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPointUint32_t dimsTest;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getDimsComplex(grid, dimsTest);
	if (dims[0] / 2 + 1 != dimsTest[0])
		hasPassed = false;
	for (int i = 1; i < NDIM; i++) {
		if (dims[i] != dimsTest[i])
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getDimsComplex_test */

extern bool
gridRegular_getPermute_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPointInt_t    permute;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getPermute(grid, permute);
	for (int i = 0; i < NDIM; i++) {
		if (permute[i] != grid->permute[i])
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getPermute_test */

extern bool
gridRegular_getNumVars_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	dataVar_t         var;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	if (gridRegular_getNumVars(grid) != 0)
		hasPassed = false;
	var = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_FPV, NDIM);
	gridRegular_attachVar(grid, var);
	if (gridRegular_getNumVars(grid) != 1)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getNumVars_test */

extern bool
gridRegular_getNumCellsTotal_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	dataVar_t         var;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	uint64_t          numCellsTotal;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	if (gridRegular_getNumVars(grid) != 0)
		hasPassed = false;
	var           = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_FPV, NDIM);
	gridRegular_attachVar(grid, var);
	numCellsTotal = 1;
	for (int i = 0; i < NDIM; i++)
		numCellsTotal *= dims[i];
	if (gridRegular_getNumCellsTotal(grid) != numCellsTotal)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getNumCellsTotal_test */

extern bool
gridRegular_attachVar_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	dataVar_t         var;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	var  = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_FPV, NDIM);
	gridRegular_attachVar(grid, var);
	if (varArr_getLength(grid->vars) != 1)
		hasPassed = false;
	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_attachVar_test */

extern bool
gridRegular_detachVar_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	dataVar_t         var;
	int               idxOfVar;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid     = local_getFakeGrid(origin, extent, dims);

	var      = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_FPV, NDIM);
	idxOfVar = gridRegular_attachVar(grid, var);
	var      = gridRegular_detachVar(grid, idxOfVar);
	if (varArr_getLength(grid->vars) != 0) {
		hasPassed = false;
	}
	gridRegular_del(&grid);
	dataVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_detachVar_test */

extern bool
gridRegular_getVarHandle_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	dataVar_t         var;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	var  = dataVar_new(LOCAL_TESTNAME, DATAVARTYPE_FPV, NDIM);
	gridRegular_attachVar(grid, var);

	if (gridRegular_getVarHandle(grid, 0) != var)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getVarHandle_test */

extern bool
gridRegular_getNumPatches_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	if (gridRegular_getNumPatches(grid) != 0)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegular_attachPatch_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPatch_t       patch;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid  = local_getFakeGrid(origin, extent, dims);
	patch = local_getFakePatch();

	if (grid->patches == NULL)
		hasPassed = false;
	gridRegular_attachPatch(grid, patch);
	if (varArr_getLength(grid->patches) != 1)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_attachPatch_test */

extern bool
gridRegular_detachPatch_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPatch_t       patch, patchTmp;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid  = local_getFakeGrid(origin, extent, dims);
	patch = local_getFakePatch();

	gridRegular_attachPatch(grid, patch);
	patchTmp = gridRegular_detachPatch(grid, 0);
	if (patch != patchTmp)
		hasPassed = false;
	if (varArr_getLength(grid->patches) != 0)
		hasPassed = false;

	gridRegular_del(&grid);
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_detachPatch_test */

extern bool
gridRegular_getPatchHandle_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPatch_t       patch, patchTmp;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid  = local_getFakeGrid(origin, extent, dims);
	patch = local_getFakePatch();

	gridRegular_attachPatch(grid, patch);
	patchTmp = gridRegular_getPatchHandle(grid, 0);
	if (patchTmp != patch)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_getPatchHandle_test */

extern bool
gridRegular_replacePatch_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPatch_t       patch, patch2, patchTmp;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid   = local_getFakeGrid(origin, extent, dims);
	patch  = local_getFakePatch();
	patch2 = local_getFakePatch();

	gridRegular_attachPatch(grid, patch);
	patchTmp = gridRegular_getPatchHandle(grid, 0);
	if (patchTmp != patch)
		hasPassed = false;
	gridRegular_replacePatch(grid, 0, patch2);
	patchTmp = gridRegular_getPatchHandle(grid, 0);
	if (patchTmp != patch2)
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_replacePatch_test */

extern bool
gridRegular_transpose_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPatch_t       patch;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid  = local_getFakeGrid(origin, extent, dims);
	patch = local_getFakePatch();
	gridRegular_attachPatch(grid, patch);

	gridRegular_transpose(grid, 0, 1);
	if (grid->dims[0] != dims[1])
		hasPassed = false;
	if (grid->dims[1] != dims[0])
		hasPassed = false;
	if (islessgreater(grid->extent[0], extent[1]))
		hasPassed = false;
	if (islessgreater(grid->origin[1], origin[0]))
		hasPassed = false;
	if ((grid->permute[0] != 1) || (grid->permute[1] != 0))
		hasPassed = false;

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_transpose_test */

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getFakeGrid(gridPointDbl_t    origin,
                  gridPointDbl_t    extent,
                  gridPointUint32_t dims)
{
	for (int i = 0; i < NDIM; i++) {
		origin[i] = -1.23 * (1 << i);
		extent[i] = 37.423 * (1 << i);
		dims[i]   = 16 * (1 << i);
	}

	return gridRegular_new(LOCAL_TESTNAME, origin, extent, dims);
}

static gridPatch_t
local_getFakePatch(void)
{
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;

	for (int i = 0; i < NDIM; i++) {
		idxLo[i] = 0;
		idxHi[i] = i + 1;
	}

	patch = gridPatch_new(idxLo, idxHi);

	return patch;
}
