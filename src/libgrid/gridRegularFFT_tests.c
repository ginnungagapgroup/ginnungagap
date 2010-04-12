// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegularFFT_tests.h"
#include "gridRegularFFT.h"
#include "gridRegular.h"
#include "gridRegularDistrib.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularFFT_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getFakeGrid(void);

static gridRegularDistrib_t
local_getFakeGridDistrib(gridRegular_t grid);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridRegularFFT_new_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularFFT_t     fft;
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid    = local_getFakeGrid();
	distrib = local_getFakeGridDistrib(grid);
	fft     = gridRegularFFT_new(grid, distrib, 0);
	if (fft->grid != grid)
		hasPassed = false;
	if (fft->gridDistrib != distrib)
		hasPassed = false;
	if (fft->idxFFTVar != 0)
		hasPassed = false;
	gridRegular_del(&grid);
	gridRegularDistrib_del(&distrib);
	gridRegularFFT_del(&fft);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegularFFT_del_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridRegularFFT_t fft;
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid    = local_getFakeGrid();
	distrib = local_getFakeGridDistrib(grid);
	fft     = gridRegularFFT_new(grid, distrib, 0);
	gridRegular_del(&grid);
	gridRegularDistrib_del(&distrib);
	gridRegularFFT_del(&fft);
	if (fft != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegularFFT_execute_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularFFT_t     fft;
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid    = local_getFakeGrid();
	distrib = local_getFakeGridDistrib(grid);
	fft     = gridRegularFFT_new(grid, distrib, 0);
	gridRegular_del(&grid);
	gridRegularDistrib_del(&distrib);
	// TODO test execute1
	gridRegularFFT_del(&fft);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getFakeGrid(void)
{
	gridRegular_t    grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridVar_t            var;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = 1.0;
		dims[i]   = 64;
	}
	var = gridVar_new("test", GRIDVARTYPE_FPV, 1);

	grid = gridRegular_new("bla", origin, extent, dims);
	gridRegular_attachVar(grid, var);

	return grid;
}

static gridRegularDistrib_t
local_getFakeGridDistrib(gridRegular_t grid)
{
	gridRegularDistrib_t distrib;
	int                  rank = 0;
#ifdef WITH_MPI
	gridPointInt_t       nProcs;
#endif
	gridPatch_t          patch;

	distrib = gridRegularDistrib_new(grid, NULL);
#ifdef WITH_MPI
	for (int i = 0; i < NDIM - 1; i++)
		nProcs[i] = 1;
	MPI_Comm_size(MPI_COMM_WORLD, &(nProcs[NDIM-1]));
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
	rank = gridRegularDistrib_getLocalRank(distrib);
#endif

	patch = gridRegularDistrib_getPatchForRank(distrib, rank);
	gridRegular_attachPatch(grid, patch);

	return distrib;
}
