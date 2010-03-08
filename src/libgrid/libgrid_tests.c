// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarTypes_tests.h"
#include "gridVar_tests.h"
#include "gridRegular_tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- Local defines -----------------------------------------------------*/
#define NAME "libgrid"


/*--- Macros ------------------------------------------------------------*/
#define RUNTEST(a, hasFailed)  \
    if (!(local_runtest(a))) { \
		hasFailed = true;      \
	} else {                   \
		if (!hasFailed)        \
			hasFailed = false; \
	}


/*--- Prototypes of loceal functions ------------------------------------*/
static bool
local_runtest(bool (*f)(void));


/*--- M A I N -----------------------------------------------------------*/
int
main(int argc, char **argv)
{
	bool hasFailed;
	int  rank = 0;
	int  size = 1;

#ifdef WITH_MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif
	if (rank == 0) {
		printf("\nTesting %s on %i %s\n",
		       NAME, size, size > 1 ? "tasks" : "task");
	}

	if (rank == 0) {
		printf("\nRunning tests for gridVarTypes:\n");
	}
	RUNTEST(&gridVarTypes_getSizePerElement_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for gridVar:\n");
	}
	RUNTEST(&gridVar_new_test, hasFailed);
	RUNTEST(&gridVar_del_test, hasFailed);
	RUNTEST(&gridVar_realloc_test, hasFailed);
	RUNTEST(&gridVar_free_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for gridRegular:\n");
	}
	RUNTEST(&gridRegular_newWithoutData_test, hasFailed);
	RUNTEST(&gridRegular_del_test, hasFailed);
	RUNTEST(&gridRegular_setOrigin_test, hasFailed);
	RUNTEST(&gridRegular_setExtent_test, hasFailed);
#ifdef WITH_MPI
	RUNTEST(&gridRegular_mpiSetDistribution_test, hasFailed);
#endif
#ifdef WITH_SILO
	RUNTEST(&gridRegular_writeSilo_test, hasFailed);
#endif

#ifdef WITH_MPI
	MPI_Finalize();
#endif

	if (hasFailed) {
		if (rank == 0)
			fprintf(stderr, "\nSome tests failed!\n\n");
		return EXIT_FAILURE;
	}
	if (rank == 0)
		printf("\nAll tests passed successfully!\n\n");
	return EXIT_SUCCESS;
} /* main */

/*--- Implementations of local functions --------------------------------*/
static bool
local_runtest(bool (*f)(void))
{
	bool hasPassed = f();
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
			printf("failed\n");
	} else {
		if (rank == 0)
			printf("passed\n");
	}

	return hasPassed;
}
