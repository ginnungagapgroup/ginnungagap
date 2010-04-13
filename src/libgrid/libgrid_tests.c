// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular_tests.h"
#include "gridRegularDistrib_tests.h"
#include "gridRegularFFT_tests.h"
#include "gridPatch_tests.h"
#include "gridVar_tests.h"
#include "gridVarType_tests.h"
#ifdef WITH_SILO
#  include "gridWriterSilo_tests.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#include "../libutil/xmem.h"
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
	bool hasFailed = false;
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
		printf("\nRunning tests for gridVar:\n");
	}
	RUNTEST(&gridVar_new_test, hasFailed);
	RUNTEST(&gridVar_del_test, hasFailed);
	RUNTEST(&gridVar_getRef_test, hasFailed);
	RUNTEST(&gridVar_getSizePerElement_test, hasFailed);
	RUNTEST(&gridVar_getNumComponents_test, hasFailed);
	RUNTEST(&gridVar_getType_test, hasFailed);
	RUNTEST(&gridVar_getMemory_test, hasFailed);
	RUNTEST(&gridVar_freeMemory_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for gridVarType:\n");
	}
	RUNTEST(&gridVarType_sizeof_test, hasFailed);
	RUNTEST(&gridVarType_isFloating_test, hasFailed);
	RUNTEST(&gridVarType_isInteger_test, hasFailed);
	RUNTEST(&gridVarType_isNativeFloat_test, hasFailed);
	RUNTEST(&gridVarType_isNativeDouble_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for gridPatch:\n");
	}
	RUNTEST(&gridPatch_new_test, hasFailed);
	RUNTEST(&gridPatch_del_test, hasFailed);
	RUNTEST(&gridPatch_getOneDim_test, hasFailed);
	RUNTEST(&gridPatch_getDims_test, hasFailed);
	RUNTEST(&gridPatch_getNumCells_test, hasFailed);
	RUNTEST(&gridPatch_getIdxLo_test, hasFailed);
	RUNTEST(&gridPatch_attachVarData_test, hasFailed);
	RUNTEST(&gridPatch_detachVarData_test, hasFailed);
	RUNTEST(&gridPatch_getVarHandle_test, hasFailed);
	RUNTEST(&gridPatch_getVarDataHandle_test, hasFailed);
	RUNTEST(&gridPatch_getNumVars_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for gridRegular:\n");
	}
	RUNTEST(&gridRegular_new_test, hasFailed);
	RUNTEST(&gridRegular_del_test, hasFailed);
	RUNTEST(&gridRegular_getRef_test, hasFailed);
	RUNTEST(&gridRegular_getName_test, hasFailed);
	RUNTEST(&gridRegular_getNumVars_test, hasFailed);
	RUNTEST(&gridRegular_attachVar_test, hasFailed);
	RUNTEST(&gridRegular_detachVar_test, hasFailed);
	RUNTEST(&gridRegular_getNumPatches_test, hasFailed);
	RUNTEST(&gridRegular_getPatchHandle_test, hasFailed);
	RUNTEST(&gridRegular_getOrigin_test, hasFailed);
	RUNTEST(&gridRegular_getDelta_test, hasFailed);
	RUNTEST(&gridRegular_getDims_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for gridRegularDistrib:\n");
	}
	RUNTEST(&gridRegularDistrib_new_test, hasFailed);
	RUNTEST(&gridRegularDistrib_del_test, hasFailed);
	RUNTEST(&gridRegularDistrib_getRef_test, hasFailed);
#ifdef WITH_MPI
	RUNTEST(&gridRegularDistrib_initMPI_test, hasFailed);
	RUNTEST(&gridRegularDistrib_getLocalRank_test, hasFailed);
#endif
	RUNTEST(&gridRegularDistrib_getPatchForRank_test, hasFailed);

	if (rank == 0) {
		printf("\nRunning tests for gridRegularFFT:\n");
	}
//	RUNTEST(&gridRegularFFT_new_test, hasFailed);
//	RUNTEST(&gridRegularFFT_del_test, hasFailed);
//	RUNTEST(&gridRegularFFT_execute_test, hasFailed);

#ifdef WITH_SILO
	if (rank == 0) {
		printf("\nRunning tests for gridWriterSilo:\n");
	}
	RUNTEST(&gridWriterSilo_new_test, hasFailed);
#ifdef WITH_MPI
	RUNTEST(&gridWriterSilo_initParallel_test, hasFailed);
#endif
	RUNTEST(&gridWriterSilo_del_test, hasFailed);
	RUNTEST(&gridWriterSilo_activate_test, hasFailed);
	RUNTEST(&gridWriterSilo_deactivate_test, hasFailed);
	RUNTEST(&gridWriterSilo_writeGridPatch_test, hasFailed);
	RUNTEST(&gridWriterSilo_writeGridRegular_test, hasFailed);
#endif

#ifdef WITH_MPI
	MPI_Finalize();
#endif

	if (hasFailed) {
		if (rank == 0)
			fprintf(stderr, "\nSome tests failed!\n\n");
		return EXIT_FAILURE;
	}
	if (rank == 0) {
		printf("\nAll tests passed successfully!\n\n");
#ifdef XMEM_TRACK_MEM
		xmem_info(stdout);
#endif
	}

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
