// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridStatistics_tests.h"
#include "gridStatistics.h"
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
#include "gridStatistics_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_FAKE_MEAN     1.0
#define LOCAL_FAKE_VARIANCE 4.0


/*--- Prototypes of local functions -------------------------------------*/
static gridPatch_t
local_getFakePatch(void);

static gridRegular_t
local_getFakeGrid(void);

static gridRegularDistrib_t
local_getFakeDistrib(void);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridStatistics_new_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridStatistics_t gridStatistics;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridStatistics = gridStatistics_new();
	gridStatistics_del(&gridStatistics);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridStatistics_del_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridStatistics_t gridStatistics;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridStatistics = gridStatistics_new();
	gridStatistics_del(&gridStatistics);
	if (gridStatistics != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridStatistics_calcGridPatch_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridStatistics_t gridStatistics;
	gridPatch_t      patch;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridStatistics = gridStatistics_new();

	patch          = local_getFakePatch();
	gridStatistics_calcGridPatch(gridStatistics, patch, 0);
	if (fabs(gridStatistics->mean - LOCAL_FAKE_MEAN) > 0.05)
		hasPassed = false;
	if (fabs(gridStatistics->var - LOCAL_FAKE_VARIANCE) > 0.10)
		hasPassed = false;
	if (fabs(gridStatistics->skew) > 0.04)
		hasPassed = false;
	if (fabs(gridStatistics->kurt) > 0.08)
		hasPassed = false;

	gridStatistics_del(&gridStatistics);
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridStatistics_calcGridPatch_test */

extern bool
gridStatistics_calcGridRegular_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridStatistics_t gridStatistics;
	gridRegular_t    grid;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridStatistics = gridStatistics_new();

	grid           = local_getFakeGrid();
	gridStatistics_calcGridRegular(gridStatistics, grid, 0);
	if (fabs(gridStatistics->mean - LOCAL_FAKE_MEAN) > 0.05)
		hasPassed = false;
	if (fabs(gridStatistics->var - LOCAL_FAKE_VARIANCE) > 0.10)
		hasPassed = false;
	if (fabs(gridStatistics->skew) > 0.04)
		hasPassed = false;
	if (fabs(gridStatistics->kurt) > 0.08)
		hasPassed = false;

	gridStatistics_del(&gridStatistics);
	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridStatistics_calcGridRegular_test */

extern bool
gridStatistics_calcGridRegularDistrib_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridStatistics_t     gridStatistics;
	gridRegularDistrib_t distrib;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridStatistics = gridStatistics_new();

	distrib        = local_getFakeDistrib();
	gridStatistics_calcGridRegularDistrib(gridStatistics, distrib, 0);
	if (fabs(gridStatistics->mean - LOCAL_FAKE_MEAN) > 0.05)
		hasPassed = false;
	if (fabs(gridStatistics->var - LOCAL_FAKE_VARIANCE) > 0.10)
		hasPassed = false;
	if (fabs(gridStatistics->skew) > 0.04)
		hasPassed = false;
	if (fabs(gridStatistics->kurt) > 0.08)
		hasPassed = false;

	gridStatistics_del(&gridStatistics);
	gridRegularDistrib_del(&distrib);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridStatistics_calcGridRegular_test */

/*--- Implementations of local functions --------------------------------*/
static gridPatch_t
local_getFakePatch(void)
{
	gridPatch_t       patch;
	gridVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	double            *data;
	uint64_t          num;
	rng_t             rng;
	int               size = 1;
	int               rank = 0;
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	var      = gridVar_new("TEST", GRIDVARTYPE_DOUBLE, 1);
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
		data[i] = rng_getGauss(rng, 0, LOCAL_FAKE_MEAN,
		                       sqrt(LOCAL_FAKE_VARIANCE));
	}
	gridVar_del(&var);
	rng_del(&rng);

	return patch;
} /* local_getFakePatch */

static gridRegular_t
local_getFakeGrid(void)
{
	gridRegular_t     grid;
	gridPatch_t       patch;
	gridPointDbl_t    origin = {0., 0., 0.};
	gridPointDbl_t    extent = {1., 1., 1.};
	gridPointUint32_t dims   = {128, 16, 128};

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
	gridVar_t            var;
	gridPointDbl_t       origin = {0., 0., 0.};
	gridPointDbl_t       extent = {1., 1., 1.};
	gridPointUint32_t    dims   = {256, 256, 256};
	gridPointInt_t       nProcs = {0, 0, 0};
	rng_t                rng;
	int                  rank = 0;
	int                  size = 1;
	double               *data;
	uint64_t             num;

	grid    = gridRegular_new("TEST", origin, extent, dims);
	var     = gridVar_new("TESTVAR", GRIDVARTYPE_DOUBLE, 1);
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
		data[i] = rng_getGauss(rng, 0, LOCAL_FAKE_MEAN,
		                       sqrt(LOCAL_FAKE_VARIANCE));
	}
	rng_del(&rng);

	gridRegular_del(&grid);

	return distrib;
} /* local_getFakeDistrib */
