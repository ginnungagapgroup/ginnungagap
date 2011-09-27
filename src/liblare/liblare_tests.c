// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lare_tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif

/*--- Local defines -----------------------------------------------------*/
#define NAME "libpart"


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

	printf("\nTesting %s\n", NAME);

	printf("\nRunning tests for lare:\n");
	RUNTEST(&lare_new_test, hasFailed);
	RUNTEST(&lare_del_test, hasFailed);
	RUNTEST(&lare_getElement_test, hasFailed);
	RUNTEST(&lare_setElement_test, hasFailed);
	RUNTEST(&lare_addElement_test, hasFailed);

	if (hasFailed) {
		fprintf(stderr, "\nSome tests failed!\n\n");
		return EXIT_FAILURE;
	}
	printf("\nAll tests passed successfully!\n\n");
	return EXIT_SUCCESS;
}

/*--- Implementations of local functions --------------------------------*/
static bool
local_runtest(bool       (*f
                       )(void))
{
	bool hasPassed = f();
	int  rank      = 0;

	if (!hasPassed) {
		if (rank == 0)
			printf("!! FAILED !!\n");
	} else {
		if (rank == 0)
			printf("passed\n");
	}

	return hasPassed;
}
