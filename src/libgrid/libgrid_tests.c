// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular_tests.h"
#include "gridRegularDistrib_tests.h"
#include "gridRegularFFT_tests.h"
#include "gridPatch_tests.h"
#include "gridUtil_tests.h"
#include "gridHistogram_tests.h"
#include "gridStatistics_tests.h"
#include "gridIO_tests.h"
#include "gridReader_tests.h"
#include "gridReaderBov_tests.h"
#ifdef WITH_SILO
#  include "gridWriterSilo_tests.h"
#  include "gridReaderSilo_tests.h"
#endif
#ifdef WITH_HDF5
#  include "gridWriterHDF5_tests.h"
#  include "gridReaderHDF5_tests.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
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


/*--- Prototypes of local functions -------------------------------------*/
static bool
local_runtest(bool (*f)(void));


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
		printf("\nRunning tests for gridUtil:\n");
	}
	RUNTEST(&gridUtil_intersection1D_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridPatch:\n");
	}
	RUNTEST(&gridPatch_new_test, hasFailed);
	RUNTEST(&gridPatch_del_test, hasFailed);
	RUNTEST(&gridPatch_getOneDim_test, hasFailed);
	RUNTEST(&gridPatch_getDims_test, hasFailed);
	RUNTEST(&gridPatch_getDimsActual_test, hasFailed);
	RUNTEST(&gridPatch_getNumCells_test, hasFailed);
	RUNTEST(&gridPatch_getIdxLo_test, hasFailed);
	RUNTEST(&gridPatch_attachVar_test, hasFailed);
	RUNTEST(&gridPatch_detachVar_test, hasFailed);
	RUNTEST(&gridPatch_allocateVarData_test, hasFailed);
	RUNTEST(&gridPatch_freeVarData_test, hasFailed);
	RUNTEST(&gridPatch_replaceVarData_test, hasFailed);
	RUNTEST(&gridPatch_getVarHandle_test, hasFailed);
	RUNTEST(&gridPatch_getVarDataHandle_test, hasFailed);
	RUNTEST(&gridPatch_getVarDataHandleByVar_test, hasFailed);
	RUNTEST(&gridPatch_getNumVars_test, hasFailed);
	RUNTEST(&gridPatch_transpose_test, hasFailed);
	RUNTEST(&gridPatch_getWindowedDataCopy_test, hasFailed);
	RUNTEST(&gridPatch_putWindowedData_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridRegular:\n");
	}
	RUNTEST(&gridRegular_new_test, hasFailed);
	RUNTEST(&gridRegular_del_test, hasFailed);
	RUNTEST(&gridRegular_getRef_test, hasFailed);
	RUNTEST(&gridRegular_getName_test, hasFailed);
	RUNTEST(&gridRegular_getOrigin_test, hasFailed);
	RUNTEST(&gridRegular_getExtent_test, hasFailed);
	RUNTEST(&gridRegular_getDelta_test, hasFailed);
	RUNTEST(&gridRegular_getDims_test, hasFailed);
	RUNTEST(&gridRegular_getDimsComplex_test, hasFailed);
	RUNTEST(&gridRegular_getPermute_test, hasFailed);
	RUNTEST(&gridRegular_getNumVars_test, hasFailed);
	RUNTEST(&gridRegular_attachVar_test, hasFailed);
	RUNTEST(&gridRegular_detachVar_test, hasFailed);
	RUNTEST(&gridRegular_getVarHandle_test, hasFailed);
	RUNTEST(&gridRegular_getNumPatches_test, hasFailed);
	RUNTEST(&gridRegular_getNumCellsTotal_test, hasFailed);
	RUNTEST(&gridRegular_attachPatch_test, hasFailed);
	RUNTEST(&gridRegular_detachPatch_test, hasFailed);
	RUNTEST(&gridRegular_getPatchHandle_test, hasFailed);
	RUNTEST(&gridRegular_replacePatch_test, hasFailed);
	RUNTEST(&gridRegular_transpose_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridRegularDistrib:\n");
	}
	RUNTEST(&gridRegularDistrib_new_test, hasFailed);
	RUNTEST(&gridRegularDistrib_del_test, hasFailed);
	RUNTEST(&gridRegularDistrib_getRef_test, hasFailed);
#ifdef WITH_MPI
	RUNTEST(&gridRegularDistrib_initMPI_test, hasFailed);
#endif
	RUNTEST(&gridRegularDistrib_getLocalRank_test, hasFailed);
	RUNTEST(&gridRegularDistrib_getPatchForRank_test, hasFailed);
	RUNTEST(&gridRegularDistrib_calcIdxsForRank1D_test, hasFailed);
	RUNTEST(&gridRegularDistrib_transpose_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridRegularFFT:\n");
	}
	RUNTEST(&gridRegularFFT_new_test, hasFailed);
	RUNTEST(&gridRegularFFT_del_test, hasFailed);
	RUNTEST(&gridRegularFFT_getNorm_test, hasFailed);
	RUNTEST(&gridRegularFFT_execute_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridHistogram:\n");
	}
	RUNTEST(&gridHistogram_new_test, hasFailed);
	RUNTEST(&gridHistogram_del_test, hasFailed);
	RUNTEST(&gridHistogram_calcGridPatch_test, hasFailed);
	RUNTEST(&gridHistogram_calcGridRegular_test, hasFailed);
#ifdef WITH_MPI
	RUNTEST(&gridHistogram_calcGridRegularDistrib_test, hasFailed);
#endif
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridStatistics:\n");
	}
	RUNTEST(&gridStatistics_new_test, hasFailed);
	RUNTEST(&gridStatistics_del_test, hasFailed);
	RUNTEST(&gridStatistics_calcGridPatch_test, hasFailed);
	RUNTEST(&gridStatistics_calcGridRegular_test, hasFailed);
#ifdef WITH_MPI
	RUNTEST(&gridStatistics_calcGridRegularDistrib_test, hasFailed);
#endif
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridIO:\n");
	}
	RUNTEST(&gridIO_getTypeFromName_test, hasFailed);
	RUNTEST(&gridIO_getNameFromType_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridReader:\n");
	}
	RUNTEST(&gridReader_newFromIni_test, hasFailed);
	RUNTEST(&gridReader_del_test, hasFailed);
	RUNTEST(&gridReader_readIntoPatch_test, hasFailed);
	RUNTEST(&gridReader_readIntoPatchForVar_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

	if (rank == 0) {
		printf("\nRunning tests for gridReaderBov:\n");
	}
	RUNTEST(&gridReaderBov_newFromIni_test, hasFailed);
	RUNTEST(&gridReaderBov_del_test, hasFailed);
	RUNTEST(&gridReaderBov_readIntoPatch_test, hasFailed);
	RUNTEST(&gridReaderBov_readIntoPatchForVar_test, hasFailed);
#ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#endif

#ifdef WITH_SILO
	if (rank == 0) {
		printf("\nRunning tests for gridWriterSilo:\n");
	}
	RUNTEST(&gridWriterSilo_new_test, hasFailed);
#  ifdef WITH_MPI
	RUNTEST(&gridWriterSilo_initParallel_test, hasFailed);
#  endif
	RUNTEST(&gridWriterSilo_del_test, hasFailed);
	RUNTEST(&gridWriterSilo_activate_test, hasFailed);
	RUNTEST(&gridWriterSilo_deactivate_test, hasFailed);
	RUNTEST(&gridWriterSilo_writeGridPatch_test, hasFailed);
	RUNTEST(&gridWriterSilo_writeGridRegular_test, hasFailed);
#  ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#  endif

	if (rank == 0) {
		printf("\nRunning tests for gridReaderSilo:\n");
	}
	RUNTEST(&gridReaderSilo_newFromIni_test, hasFailed);
	RUNTEST(&gridReaderSilo_del_test, hasFailed);
#  ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#  endif
#endif
#ifdef WITH_HDF5
	if (rank == 0) {
		printf("\nRunning tests for gridWriterHDF5:\n");
	}
	RUNTEST(&gridWriterHDF5_new_test, hasFailed);
#  ifdef WITH_MPI
//	RUNTEST(&gridWriterHDF5_initParallel_test, hasFailed);
#  endif
	RUNTEST(&gridWriterHDF5_del_test, hasFailed);
#if 0
	RUNTEST(&gridWriterHDF5_activate_test, hasFailed);
	RUNTEST(&gridWriterHF5_deactivate_test, hasFailed);
	RUNTEST(&gridWriterHDF5_writeGridPatch_test, hasFailed);
#endif
	RUNTEST(&gridWriterHDF5_writeGridRegular_test, hasFailed);
#  ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#  endif

#if 0
	if (rank == 0) {
		printf("\nRunning tests for gridReaderHDF5:\n");
	}
	RUNTEST(&gridReaderHDF5_newFromIni_test, hasFailed);
	RUNTEST(&gridReaderHDF5_del_test, hasFailed);
	RUNTEST(&gridReaderHDF5_readIntoPatch_test, hasFailed);
	RUNTEST(&gridReaderHDF5_readIntoPatchForVar_test, hasFailed);
#  ifdef XMEM_TRACK_MEM
	if (rank == 0)
		xmem_info(stdout);
	global_max_allocated_bytes = 0;
#  endif
#endif
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
			printf("!! FAILED !!\n");
	} else {
		if (rank == 0)
			printf("passed\n");
	}

	return hasPassed;
}
