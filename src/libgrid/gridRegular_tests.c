// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular_tests.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include <inttypes.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <string.h>
#ifdef WITH_SILO
#  include <silo.h>
#endif


/*--- Local defines -----------------------------------------------------*/
#define TESTNAME "testname"


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegular_adt.h"


/*--- Local defines -----------------------------------------------------*/
static gridRegular_t
local_getFakeGrid(gridPointSize_t *dims);


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridRegular_newWithoutData_test(void)
{
	bool            hasPassed = true;
	gridRegular_t   grid;
	gridPointSize_t dims;
	int             rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(&dims);

	if (strcmp(TESTNAME, grid->gridName) != 0)
		hasPassed = false;
	for (int i = 0; i < NDIM; i++) {
		if (dims[i] != grid->globalDims[i])
			hasPassed = false;
		if (islessgreater(0.0, grid->globalOrigin[i]))
			hasPassed = false;
		if (islessgreater(1.0, grid->globalExtent[i]))
			hasPassed = false;
	}

	gridRegular_del(&grid);

	return hasPassed ? true : false;
}

extern bool
gridRegular_del_test(void)
{
	bool            hasPassed = true;
	gridRegular_t   grid;
	gridPointSize_t dims;
	int             rank = 0;
#ifdef WITH_PROC_DIR
	FILE            *pf;
	unsigned        sizeOld, sizeNew;
#endif

#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

#ifdef WITH_PROC_DIR
	pf = fopen("/proc/self/statm", "r");
	fscanf(pf, "%u", &sizeOld);
	rewind(pf);
#endif

	grid = local_getFakeGrid(&dims);
	gridRegular_del(&grid);

	if (grid != NULL)
		hasPassed = false;

#ifdef WITH_PROC_DIR
	fscanf(pf, "%u", &sizeNew);
	if (sizeOld != sizeNew)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegular_del_test */

extern bool
gridRegular_setOrigin_test(void)
{
	bool            hasPassed = true;
	gridRegular_t   grid;
	gridPointSize_t dims;
	int             i;
	gridPointDbl_t  origin;
	int             rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(&dims);
	for (i = 0; i < NDIM; i++)
		origin[i] = 2.0;

	gridRegular_setOrigin(grid, origin);

	for (i = 0; i < NDIM; i++) {
		if (islessgreater(grid->globalOrigin[i], origin[i]))
			hasPassed = false;
	}

	return hasPassed ? true : false;
}

extern bool
gridRegular_setExtent_test(void)
{
	bool            hasPassed = true;
	gridRegular_t   grid;
	gridPointSize_t dims;
	int             i;
	gridPointDbl_t  extent;
	gridPointDbl_t  oldDelta;
	int             rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(&dims);
	for (i = 0; i < NDIM; i++) {
		extent[i]   = grid->globalExtent[i] * 2.0;
		oldDelta[i] = grid->delta[i];
	}

	gridRegular_setExtent(grid, extent);

	for (i = 0; i < NDIM; i++) {
		if (islessgreater(grid->globalExtent[i], extent[i]))
			hasPassed = false;
		if (isgreater(fabs(grid->delta[i] - 2.0 * oldDelta[i]),
		              DBL_EPSILON))
			hasPassed = false;
	}

	return hasPassed ? true : false;
} /* gridRegular_setExtent_test */

#ifdef WITH_MPI
extern bool
gridRegular_mpiSetDistribution_test(void)
{
	bool            hasPassed = true;
	gridRegular_t   grid;
	gridPointSize_t dims;
	gridPointInt_t  nProcs;
	int             i;
	int             rank = 0;
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid      = local_getFakeGrid(&dims);
	nProcs[0] = 4;
	for (i = 1; i < NDIM; i++)
		nProcs[i] = 0;
	gridRegular_mpiSetDistribution(grid, nProcs, MPI_COMM_WORLD);

	if (grid->mpiNumProcs[0] != nProcs[0])
		hasPassed = false;
	if ((grid->localDims[0] < grid->globalDims[0] / nProcs[0])
	    || (grid->localDims[0] > grid->globalDims[0] / nProcs[0] + 1))
		hasPassed = false;

	gridRegular_del(&grid);


	return hasPassed ? true : false;
} /* gridRegular_mpiSetDistribution_test */

#endif


#ifdef WITH_SILO
extern bool
gridRegular_writeSilo_test(void)
{
	bool hasPassed = true;
	gridRegular_t   grid;
	gridPointSize_t dims;
	int  rank      = 0;
	gridPointDbl_t  extent;
#  ifdef WITH_MPI
	gridPointInt_t  nProcs;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	nProcs[NDIM-1] = 4;
	for (int i = 0; i < NDIM-1; i++)
		nProcs[i] = 0;
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid = local_getFakeGrid(&dims);
	for (int i=0; i<NDIM; i++)
		extent[i] = (double)(1<<i);
	gridRegular_setExtent(grid, extent);
#ifdef WITH_MPI
	gridRegular_mpiSetDistribution(grid, nProcs, MPI_COMM_WORLD);
#endif
	gridRegular_writeSilo(grid, "test");
	gridRegular_del(&grid);

	return hasPassed ? true : false;
}

#endif


/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getFakeGrid(gridPointSize_t *dims)
{
	gridRegular_t grid;

	for (int i = 0; i < NDIM; i++) {
		(*dims)[i] = (gridSize_t)(64 * (1 << i));
	}
	grid = gridRegular_newWithoutData(TESTNAME, *dims);

	return grid;
}
