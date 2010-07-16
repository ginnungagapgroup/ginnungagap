// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "refCounter_tests.h"
#include "xstring_tests.h"
#include "varArr_tests.h"
#include "endian_tests.h"
#include "bov_tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include "commSchemeBuffer_tests.h"
#  include "commScheme_tests.h"
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif

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

#ifdef WITH_MPI
#  define RUNTESTMPI(a, hasFailed) \
    if (!(local_runtestMPI(a))) {  \
		hasFailed = true;          \
	} else {                       \
		if (!hasFailed)            \
			hasFailed = false;     \
	}
#endif


/*--- Prototypes of loceal functions ------------------------------------*/
static bool
local_runtest(bool (*f)(void));


#ifdef WITH_MPI
static bool
local_runtestMPI(bool (*f)(void));

#endif


/*--- M A I N -----------------------------------------------------------*/
int
main(int argc, char **argv)
{
	bool hasFailed = false;
	int  rank      = 0;
	int  size      = 1;

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
		printf("\nRunning tests for refCounter:\n");
		RUNTEST(&refCounter_init_test, hasFailed);
		RUNTEST(&refCounter_ref_test, hasFailed);
		RUNTEST(&refCounter_deref_test, hasFailed);
		RUNTEST(&refCounter_noReferenceLeft_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for xstring:\n");
		RUNTEST(&xstring_xdirname_test, hasFailed);
		RUNTEST(&xstring_xbasename_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for endian:\n");
		RUNTEST(&endian_fileIsLittleByBlock_test, hasFailed);
		RUNTEST(&endian_fileIsBigByBlock_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for varArr:\n");
		RUNTEST(&varArr_new_test, hasFailed);
		RUNTEST(&varArr_del_test, hasFailed);
		RUNTEST(&varArr_getLength_test, hasFailed);
		RUNTEST(&varArr_insert_test, hasFailed);
		RUNTEST(&varArr_remove_test, hasFailed);
		RUNTEST(&varArr_replace_test, hasFailed);
		RUNTEST(&varArr_getElementHandle_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for bov:\n");
		RUNTEST(&bov_new_test, hasFailed);
		RUNTEST(&bov_newFromFile_test, hasFailed);
		RUNTEST(&bov_del_test, hasFailed);
		RUNTEST(&bov_getTime_test, hasFailed);
		RUNTEST(&bov_getDataFileName_test, hasFailed);
		RUNTEST(&bov_getDataSize_test, hasFailed);
		RUNTEST(&bov_getDataFormat_test, hasFailed);
		RUNTEST(&bov_getVarName_test, hasFailed);
		RUNTEST(&bov_getDataEndian_test, hasFailed);
		RUNTEST(&bov_getCentering_test, hasFailed);
		RUNTEST(&bov_getBrickOrigin_test, hasFailed);
		RUNTEST(&bov_getBrickSize_test, hasFailed);
		RUNTEST(&bov_getDataComponents_test, hasFailed);
		RUNTEST(&bov_setTime_test, hasFailed);
		RUNTEST(&bov_setDataFileName_test, hasFailed);
		RUNTEST(&bov_setDataSize_test, hasFailed);
		RUNTEST(&bov_setDataFormat_test, hasFailed);
		RUNTEST(&bov_setVarName_test, hasFailed);
		RUNTEST(&bov_setDataEndian_test, hasFailed);
		RUNTEST(&bov_setCentering_test, hasFailed);
		RUNTEST(&bov_setBrickOrigin_test, hasFailed);
		RUNTEST(&bov_setBrickSize_test, hasFailed);
		RUNTEST(&bov_setDataComponents_test, hasFailed);
		RUNTEST(&bov_read_test, hasFailed);
		RUNTEST(&bov_readWindowed_test, hasFailed);
	}


#ifdef WITH_MPI
	MPI_Barrier(MPI_COMM_WORLD);
	if (rank == 0) {
		printf("\nRunning tests for commSchemeBuffer:\n");
	}
	RUNTESTMPI(&commSchemeBuffer_new_test, hasFailed);
	RUNTESTMPI(&commSchemeBuffer_del_test, hasFailed);
	RUNTESTMPI(&commSchemeBuffer_getBuf_test, hasFailed);
	RUNTESTMPI(&commSchemeBuffer_getCount_test, hasFailed);
	RUNTESTMPI(&commSchemeBuffer_getDatatype_test, hasFailed);
	RUNTESTMPI(&commSchemeBuffer_getRank_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for commScheme:\n");
	}
	RUNTESTMPI(&commScheme_new_test, hasFailed);
	RUNTESTMPI(&commScheme_del_test, hasFailed);
	RUNTESTMPI(&commScheme_addBuffer_test, hasFailed);
	RUNTESTMPI(&commScheme_fire_test, hasFailed);
	RUNTESTMPI(&commScheme_fireBlock_test, hasFailed);
	RUNTESTMPI(&commScheme_wait_test, hasFailed);

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

	if (!hasPassed) {
		if (rank == 0)
			printf("!! FAILED !!\n");
	} else {
		if (rank == 0)
			printf("passed\n");
	}

	return hasPassed;
}

#ifdef WITH_MPI
static bool
local_runtestMPI(bool (*f)(void))
{
	bool hasPassed   = f();
	int  rank        = 0;
	int  failedGlobal;
	int  failedLocal = hasPassed ? 0 : 1;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Allreduce(&failedLocal, &failedGlobal, 1, MPI_INT, MPI_MAX,
	              MPI_COMM_WORLD);
	if (failedGlobal != 0)
		hasPassed = false;

	if (!hasPassed) {
		if (rank == 0)
			printf("!! FAILED !!\n");
	} else {
		if (rank == 0)
			printf("passed\n");
	}

	return hasPassed;
}

#endif
