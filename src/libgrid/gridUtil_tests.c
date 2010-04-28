// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridUtil_tests.h"
#include "gridUtil.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridUtil_intersection1D_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint32_t sA[2], sB[2], sC[2];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	sA[0] = 10;
	sA[1] = 20;

	sB[0] = 15;
	sB[1] = 25;
	if(!gridUtil_intersection1D(sA[0], sA[1], sB[0], sB[1], sC, sC + 1))
		hasPassed = false;
	if (sC[0] != 15 && sC[1] != 20)
		hasPassed = false;

	sB[0] = 5;
	sB[1] = 25;
	if(!gridUtil_intersection1D(sA[0], sA[1], sB[0], sB[1], sC, sC + 1))
		hasPassed = false;
	if (sC[0] != 10 && sC[1] != 20)
		hasPassed = false;

	sB[0] = 5;
	sB[1] = 15;
	if(!gridUtil_intersection1D(sA[0], sA[1], sB[0], sB[1], sC, sC + 1))
		hasPassed = false;
	if (sC[0] != 10 && sC[1] != 15)
		hasPassed = false;

	sB[0] = 11;
	sB[1] = 19;
	if(!gridUtil_intersection1D(sA[0], sA[1], sB[0], sB[1], sC, sC + 1))
		hasPassed = false;
	if (sC[0] != 11 && sC[1] != 19)
		hasPassed = false;

	sB[0] = 1;
	sB[1] = 2;
	if(gridUtil_intersection1D(sA[0], sA[1], sB[0], sB[1], sC, sC + 1))
		hasPassed = false;

	sB[0] = 25;
	sB[1] = 35;
	if(gridUtil_intersection1D(sA[0], sA[1], sB[0], sB[1], sC, sC + 1))
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
