// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/timer.c
 * @ingroup libutilMisc
 * @brief  This file provides the implementation of the timer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "timer.h"
#include <stdio.h>
#include <assert.h>
#if (defined WITH_MPI)
#  include <mpi.h>
#elif (defined _OPENMP)
#  include <omp.h>
#else
#  include <time.h>
#endif


/*--- Local variables ---------------------------------------------------*/
#if (!defined WITH_MPI && !defined _OPENMP)
static double CPS_INV = 1. / ((double)CLOCKS_PER_SEC);
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern double
timer_start(const char *text)
{
	double timing;
	int    rank = 0;

#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0) {
		printf("%s... ", text);
		fflush(stdout);
	}

#if (defined WITH_MPI)
	MPI_Barrier(MPI_COMM_WORLD);
	timing = -MPI_Wtime();
#elif (defined _OPENMP)
	timing = -omp_get_wtime();
#else
	timing = -clock() * CPS_INV;
#endif

	return timing;
}

extern double
timer_stop(double timing)
{
	int rank = 0;

#if (defined WITH_MPI)
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	timing += MPI_Wtime();
	{
		double timingMax;
		MPI_Allreduce(&timing, &timingMax, 1, MPI_DOUBLE, MPI_MAX,
		              MPI_COMM_WORLD);
		timing = timingMax;
	}
	MPI_Barrier(MPI_COMM_WORLD);
#elif (defined _OPENMP)
	timing += omp_get_wtime();
#else
	timing += clock() * CPS_INV;
#endif

	if (rank == 0) {
		printf("took %.5fs\n", timing);
		fflush(stdout);
	}

	return timing;
}

/*--- Implementations of local functions --------------------------------*/
