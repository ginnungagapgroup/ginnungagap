// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridHistogram_tests.h"
#include "gridHistogram.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridPatch.h"
#include "gridRegular.h"
#include "../libutil/rng.h"
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridHistogram_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridPatch_t
local_getFakePatch(void);

static gridRegular_t
local_getFakeGrid(void);

static gridRegularDistrib_t
local_getFakeDistrib(void);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridHistogram_new_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	gridHistogram_t gridHistogram;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridHistogram = gridHistogram_new(4, 0., 1.);
	gridHistogram_del(&gridHistogram);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridHistogram_del_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	gridHistogram_t gridHistogram;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridHistogram = gridHistogram_new(4, 0., 1.0);
	gridHistogram_del(&gridHistogram);
	if (gridHistogram != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridHistogram_calcGridPatch_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	gridHistogram_t gridHistogram;
	gridPatch_t     patch;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	patch         = local_getFakePatch();
	gridHistogram = gridHistogram_new(7, -3.0, 3.0);
	gridHistogram_calcGridPatch(gridHistogram, patch, 0);

	if ((gridHistogram->binCounts[4] < gridHistogram->binCounts[5])
	    || (gridHistogram->binCounts[4] < gridHistogram->binCounts[3]))
		hasPassed = false;
	if (fabs((double)(gridHistogram->binCounts[5])
	         / ((double)(gridHistogram->binCounts[3])) - 1.0) > 0.015)
		hasPassed = false;
	if (fabs((double)(gridHistogram->binCounts[6])
	         / ((double)(gridHistogram->binCounts[2])) - 1.0) > 0.05)
		hasPassed = false;

	gridHistogram_del(&gridHistogram);
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridHistogram_calcGridPatch_test */

extern bool
gridHistogram_calcGridRegular_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	gridHistogram_t gridHistogram;
	gridRegular_t   grid;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid          = local_getFakeGrid();
	gridHistogram = gridHistogram_new(7, -3.0, 3.0);
	gridHistogram_calcGridRegular(gridHistogram, grid, 0);

	if ((gridHistogram->binCounts[4] < gridHistogram->binCounts[5])
	    || (gridHistogram->binCounts[4] < gridHistogram->binCounts[3]))
		hasPassed = false;
	if (fabs((double)(gridHistogram->binCounts[5])
	         / ((double)(gridHistogram->binCounts[3])) - 1.0) > 0.015)
		hasPassed = false;
	if (fabs((double)(gridHistogram->binCounts[6])
	         / ((double)(gridHistogram->binCounts[2])) - 1.0) > 0.05)
		hasPassed = false;

	gridHistogram_del(&gridHistogram);
	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridHistogram_calcGridRegular_test */

extern bool
gridHistogram_calcGridRegularDistrib_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridHistogram_t      gridHistogram;
	gridRegularDistrib_t distrib;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	distrib       = local_getFakeDistrib();
	gridHistogram = gridHistogram_new(7, -3.0, 3.0);
	gridHistogram_calcGridRegularDistrib(gridHistogram, distrib, 0);

	if ((gridHistogram->binCounts[4] < gridHistogram->binCounts[5])
	    || (gridHistogram->binCounts[4] < gridHistogram->binCounts[3]))
		hasPassed = false;
	if (fabs((double)(gridHistogram->binCounts[5])
	         / ((double)(gridHistogram->binCounts[3])) - 1.0) > 0.01)
		hasPassed = false;
	if (fabs((double)(gridHistogram->binCounts[6])
	         / ((double)(gridHistogram->binCounts[2])) - 1.0) > 0.05)
		hasPassed = false;

	gridHistogram_del(&gridHistogram);
	gridRegularDistrib_del(&distrib);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridHistogram_calcGridRegularDistrib_test */

/*--- Implementations of local functions --------------------------------*/
static gridPatch_t
local_getFakePatch(void)
{
	gridPatch_t       patch;
	dataVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	double            *data;
	uint64_t          num;
	rng_t             rng;
	int               size = 1;
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif

	var      = dataVar_new("TEST", DATAVARTYPE_DOUBLE, 1);
	idxLo[0] = 0;
	idxHi[0] = 15;
	idxLo[1] = 0;
	idxHi[1] = 127;
#if (NDIM > 2)
	idxLo[2] = 0;
	idxHi[2] = 127;
#endif
	patch    = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(patch, var);

	data = gridPatch_getVarDataHandle(patch, 0);
	rng  = rng_new(3, size, 45452);
	num  = gridPatch_getNumCells(patch);
	for (uint64_t i = 0; i < num; i++) {
		data[i] = rng_getGauss(rng, 0, 0.0, 1.0);
	}
	dataVar_del(&var);
	rng_del(&rng);

	return patch;
} /* local_getFakePatch */

static gridRegular_t
local_getFakeGrid(void)
{
	gridRegular_t     grid;
	gridPatch_t       patch;
	gridPointDbl_t    origin = { 0., 0., 0. };
	gridPointDbl_t    extent = { 1., 1., 1. };
	gridPointUint32_t dims   = { 128, 16, 128 };

	patch = local_getFakePatch();
	grid  = gridRegular_new("TEST", origin, extent, dims);
	gridRegular_attachPatch(grid, patch);

	return grid;
}

static gridRegularDistrib_t
local_getFakeDistrib(void)
{
	gridRegularDistrib_t distrib;
	gridRegular_t        grid;
	gridPatch_t          patch;
	dataVar_t            var;
	gridPointDbl_t       origin = { 0., 0., 0. };
	gridPointDbl_t       extent = { 1., 1., 1. };
	gridPointUint32_t    dims   = { 256, 256, 256 };
	gridPointInt_t       nProcs = { 0, 0, 0 };
	rng_t                rng;
	int                  rank   = 0;
	int                  size   = 1;
	double               *data;
	uint64_t             num;

	grid    = gridRegular_new("TEST", origin, extent, dims);
	var     = dataVar_new("TESTVAR", DATAVARTYPE_DOUBLE, 1);
	gridRegular_attachVar(grid, var);
	distrib = gridRegularDistrib_new(grid, nProcs);
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
	rank = gridRegularDistrib_getLocalRank(distrib);
#endif

	patch = gridRegularDistrib_getPatchForRank(distrib, rank);
	gridRegular_attachPatch(grid, patch);
	data  = gridPatch_getVarDataHandle(patch, 0);
	rng   = rng_new(3, size, 45452);
	num   = gridPatch_getNumCells(patch);
	for (uint64_t i = 0; i < num; i++) {
		data[i] = rng_getGauss(rng, 0, 0.0, 1.0);
	}
	rng_del(&rng);

	gridRegular_del(&grid);

	return distrib;
} /* local_getFakeDistrib */
