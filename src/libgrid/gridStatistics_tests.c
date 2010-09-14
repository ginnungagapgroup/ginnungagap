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
#include "../libutil/rng.h"
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridStatistics_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_FAKE_MEAN 1.0
#define LOCAL_FAKE_VARIANCE 4.0


/*--- Prototypes of local functions -------------------------------------*/
static gridPatch_t
local_getFakePatch(void);


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
}

/*--- Implementations of local functions --------------------------------*/
static gridPatch_t
local_getFakePatch(void)
{
	gridPatch_t       patch;
	gridVar_t         var;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	double             *data;
	uint64_t          num;
	rng_t             rng;
	int size = 1;
	int rank = 0;
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
	data     = gridPatch_getVarDataHandle(patch, 0);
	rng      = rng_new(3, size, 45452);
	num      = gridPatch_getNumCells(patch);
	for (uint64_t i = 0; i < num; i++) {
		data[i] = rng_getGauss(rng, 0, LOCAL_FAKE_MEAN,
		                       sqrt(LOCAL_FAKE_VARIANCE));
	}
	gridVar_del(&var);
	rng_del(&rng);

	return patch;
}
