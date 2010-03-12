// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular_tests.h"
#include "gridRegular.h"
#include "gridVar.h"
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
gridRegular_attachVar_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridVar_t         var;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	var         = gridVar_new(LOCAL_TESTNAME, GRIDVARTYPE_FPV, NDIM);
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
	gridVar_t         var;
	int               idxOfVar;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	var         = gridVar_new(LOCAL_TESTNAME, GRIDVARTYPE_FPV, NDIM);
	idxOfVar    = gridRegular_attachVar(grid, var);
	var         = gridRegular_detachVar(grid, idxOfVar);
	if (varArr_getLength(grid->vars) != 0) {
		hasPassed = false;
	}
	gridRegular_del(&grid);
	gridVar_del(&var);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_detachVar_test */

extern bool
gridRegular_getNumPatches_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
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
gridRegular_getPatchHandle_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	// TODO Test the function!

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
	bool   hasPassed      = true;
	int    rank           = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    origin2;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getOrigin(grid, origin2);
	for (int i=0; i<NDIM; i++) {
		if (islessgreater(origin2[i], grid->origin[i]))
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegular_getDelta_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointDbl_t    delta;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(origin, extent, dims);

	gridRegular_getDelta(grid, delta);
	for (int i=0; i<NDIM; i++) {
		if (islessgreater(delta[i], grid->delta[i]))
			hasPassed = false;
	}

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

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
