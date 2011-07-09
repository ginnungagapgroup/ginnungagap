// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/myTest.c
 * @ingroup libutilMiscMyTest
 * @brief  This file implements the code test infrastructure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "myTest.h"
#include <stdio.h>
#include <stdbool.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- Implementation of exported functions ------------------------------*/

extern bool
myTest_runtest(myTest_testFunc_t functionToTest)
{
	bool hasPassed = functionToTest();
	int  rank      = 0;
#ifdef WITH_MPI
	int  failedGlobal;
	int  failedLocal = hasPassed ? 0 : 1;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Allreduce(&failedLocal, &failedGlobal, 1, MPI_INT, MPI_MAX,
	              MPI_COMM_WORLD);
	if (failedGlobal != 0)
		hasPassed = false;
#endif

	if (!hasPassed) {
		if (rank == 0)
			printf("!! FAILED !!\n");
	} else {
		if (rank == 0)
			printf("passed\n");
	}

	return hasPassed;
}
