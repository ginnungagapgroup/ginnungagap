// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "refCounter_tests.h"
#include "varArr_tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#undef WITH_MPI

/*--- Local defines -----------------------------------------------------*/
#define NAME "libutil"


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
	bool hasFailed = false;
	int  rank      = 0;
	int  size      = 1;

	if (rank == 0) {
		printf("\nTesting %s on %i %s\n",
		       NAME, size, size > 1 ? "tasks" : "task");
	}

	if (rank == 0) {
		printf("\nRunning tests for refCounter:\n");
	}
	RUNTEST(&refCounter_init_test, hasFailed);
	RUNTEST(&refCounter_ref_test, hasFailed);
	RUNTEST(&refCounter_deref_test, hasFailed);
	RUNTEST(&refCounter_noReferenceLeft_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for varArr:\n");
	}
	RUNTEST(&varArr_new_test, hasFailed);
	RUNTEST(&varArr_del_test, hasFailed);
	RUNTEST(&varArr_getLength_test, hasFailed);
	RUNTEST(&varArr_insert_test, hasFailed);
	RUNTEST(&varArr_remove_test, hasFailed);
	RUNTEST(&varArr_getElementHandle_test, hasFailed);

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
			printf("!! FAILED !!\n");
	} else {
		if (rank == 0)
			printf("passed\n");
	}

	return hasPassed;
}
