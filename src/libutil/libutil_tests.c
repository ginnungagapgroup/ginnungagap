// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of 'ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "refCounter_tests.h"
#include "xstring_tests.h"
#include "stai_tests.h"
#include "varArr_tests.h"
#include "endian_tests.h"
#include "tile_tests.h"
#include "lIdx_tests.h"
#include "filename_tests.h"
#include "bov_tests.h"
#include "grafic_tests.h"
#include "cubepm_tests.h"
#include "gadgetVersion_tests.h"
#include "gadgetBlock_tests.h"
#include "gadgetTOC_tests.h"
#include "gadgetHeader_tests.h"
#include "gadget_tests.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include "commSchemeBuffer_tests.h"
#  include "commScheme_tests.h"
#  include "groupi_tests.h"
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "xmem.h"
#endif

/*--- Local defines -----------------------------------------------------*/
#define NAME "libutil"


/*--- Macros ------------------------------------------------------------*/
#define RUNTEST(a, hasFailed)   \
    if (!(local_runtest(a))) {  \
		hasFailed = true;       \
	} else {                    \
		if (!hasFailed)         \
			hasFailed = false;  \
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
local_runtest(bool       (*f
                       )(void));


#ifdef WITH_MPI
static bool
local_runtestMPI(bool       (*f
                          )(void));

#endif


/*--- M A I N -----------------------------------------------------------*/
int
main(void)
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
		RUNTEST(&endian_getFileEndianessByBlock_test, hasFailed);
		RUNTEST(&endian_getFileEndianessByBlockF_test, hasFailed);
		RUNTEST(&endian_fileIsLittleByBlock_test, hasFailed);
		RUNTEST(&endian_fileIsBigByBlock_test, hasFailed);
		RUNTEST(&endian_fileIsLittleByBlockF_test, hasFailed);
		RUNTEST(&endian_fileIsBigByBlockF_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for tile:\n");
		RUNTEST(&tile_calcNDIdxsELAE_test, hasFailed);
		RUNTEST(&tile_calcIdxsELAE_test, hasFailed);
		RUNTEST(&tile_calcTileNumberForIdxELAE_test, hasFailed);
		RUNTEST(&tile_calcNDIdxsELAB_test, hasFailed);
		RUNTEST(&tile_calcIdxsELAB_test, hasFailed);
		RUNTEST(&tile_calcTileNumberForIdxELAB_test, hasFailed);
		RUNTEST(&tile_calcNumLargeTilesEven_test, hasFailed);
		RUNTEST(&tile_calcNumSmallTilesEven_test, hasFailed);
		RUNTEST(&tile_calcMinTileSizeEven_test, hasFailed);
		RUNTEST(&tile_calcMaxTileSizeEven_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for lIdx:\n");
		RUNTEST(&lIdx_fromCoord2d_test, hasFailed);
		RUNTEST(&lIdx_toCoord2d_test, hasFailed);
		RUNTEST(&lIdx_fromCoord3d_test, hasFailed);
		RUNTEST(&lIdx_toCoord3d_test, hasFailed);
		RUNTEST(&lIdx_fromCoordNd_test, hasFailed);
		RUNTEST(&lIdx_toCoordNd_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for filename:\n");
		RUNTEST(&filename_new_test, hasFailed);
		RUNTEST(&filename_newFull_test, hasFailed);
		RUNTEST(&filename_clone_test, hasFailed);
		RUNTEST(&filename_del_test, hasFailed);
		RUNTEST(&filename_setPath_test, hasFailed);
		RUNTEST(&filename_setPrefix_test, hasFailed);
		RUNTEST(&filename_setQualifier_test, hasFailed);
		RUNTEST(&filename_setSuffix_test, hasFailed);
		RUNTEST(&filename_getFullName_test, hasFailed);
		RUNTEST(&filename_copySetFields_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for stai:\n");
		RUNTEST(&stai_new_test, hasFailed);
		RUNTEST(&stai_clone_test, hasFailed);
		RUNTEST(&stai_cloneWithDifferentBase_test, hasFailed);
		RUNTEST(&stai_del_test, hasFailed);
		RUNTEST(&stai_setElement_test, hasFailed);
		RUNTEST(&stai_setElementsMulti_test, hasFailed);
		RUNTEST(&stai_getElement_test, hasFailed);
		RUNTEST(&stai_getElementsMulti_test, hasFailed);
		RUNTEST(&stai_isLinear_test, hasFailed);
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

	if (rank == 0) {
		printf("\nRunning tests for grafic:\n");
		RUNTEST(&grafic_new_test, hasFailed);
		RUNTEST(&grafic_newFromFile_test, hasFailed);
		RUNTEST(&grafic_del_test, hasFailed);
		RUNTEST(&grafic_getFileName_test, hasFailed);
		RUNTEST(&grafic_getSize_test, hasFailed);
		RUNTEST(&grafic_getDx_test, hasFailed);
		RUNTEST(&grafic_getXoff_test, hasFailed);
		RUNTEST(&grafic_getAstart_test, hasFailed);
		RUNTEST(&grafic_getOmegam_test, hasFailed);
		RUNTEST(&grafic_getOmegav_test, hasFailed);
		RUNTEST(&grafic_getH0_test, hasFailed);
		RUNTEST(&grafic_getIseed_test, hasFailed);
		RUNTEST(&grafic_getIsWhiteNoise_test, hasFailed);
		RUNTEST(&grafic_setFileName_test, hasFailed);
		RUNTEST(&grafic_setSize_test, hasFailed);
		RUNTEST(&grafic_setDx_test, hasFailed);
		RUNTEST(&grafic_setXoff_test, hasFailed);
		RUNTEST(&grafic_setAstart_test, hasFailed);
		RUNTEST(&grafic_setOmegam_test, hasFailed);
		RUNTEST(&grafic_setOmegav_test, hasFailed);
		RUNTEST(&grafic_setH0_test, hasFailed);
		RUNTEST(&grafic_setIseed_test, hasFailed);
		RUNTEST(&grafic_setIsWhiteNoise_test, hasFailed);
		RUNTEST(&grafic_makeEmptyFile_test, hasFailed);
		RUNTEST(&grafic_read_test, hasFailed);
		RUNTEST(&grafic_readWindowed_test, hasFailed);
		RUNTEST(&grafic_write_test, hasFailed);
		RUNTEST(&grafic_writeWindowed_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for cubepm:\n");
		RUNTEST(&cubepm_new_test, hasFailed);
		RUNTEST(&cubepm_del_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for gadgetVersion:\n");
		RUNTEST(&gadgetVersion_getVersionFromFile_test, hasFailed);
		RUNTEST(&gadgetVersion_getNameFromType_test, hasFailed);
		RUNTEST(&gadgetVersion_getTypeFromName_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for gadgetBlock:\n");
		RUNTEST(&gadgetBlock_getNameFromType_test, hasFailed);
		RUNTEST(&gadgetBlock_getTypeFromName_test, hasFailed);
		RUNTEST(&gadgetBlock_getNumComponents_test, hasFailed);
		RUNTEST(&gadgetBlock_isInteger_test, hasFailed);
		RUNTEST(&gadgetBlock_getNumPartsInBlock_test, hasFailed);
		RUNTEST(&gadgetBlock_writereadDescriptor_test, hasFailed);
		RUNTEST(&gadgetBlock_readDescriptorString_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for gadgetTOC:\n");
		RUNTEST(&gadgetTOC_new_test, hasFailed);
		RUNTEST(&gadgetTOC_newFromFile_test, hasFailed);
		RUNTEST(&gadgetTOC_clone_test, hasFailed);
		RUNTEST(&gadgetTOC_del_test, hasFailed);
		RUNTEST(&gadgetTOC_setFileVersion_test, hasFailed);
		RUNTEST(&gadgetTOC_getFileVersion_test, hasFailed);
		RUNTEST(&gadgetTOC_blockExists_test, hasFailed);
		RUNTEST(&gadgetTOC_blockExistsByName_test, hasFailed);
		RUNTEST(&gadgetTOC_getOffsetForBlock_test, hasFailed);
		RUNTEST(&gadgetTOC_getSizeInBytesForBlock_test, hasFailed);
		RUNTEST(&gadgetTOC_getNameInV2FilesForBlock_test, hasFailed);
		RUNTEST(&gadgetTOC_getOffsetBySeqNumber_test, hasFailed);
		RUNTEST(&gadgetTOC_getTypeBySeqNumber_test, hasFailed);
		RUNTEST(&gadgetTOC_getSizeInBytesBySeqNumber_test, hasFailed);
		RUNTEST(&gadgetTOC_getNameBySeqNumber_test, hasFailed);
		RUNTEST(&gadgetTOC_getTotalFileSize_test, hasFailed);
		RUNTEST(&gadgetTOC_addEntryByName_test, hasFailed);
		RUNTEST(&gadgetTOC_addEntryByType_test, hasFailed);
		RUNTEST(&gadgetTOC_calcSizes_test, hasFailed);
		RUNTEST(&gadgetTOC_calcOffset_test, hasFailed);
		RUNTEST(&gadgetTOC_seekToData_test, hasFailed);
		RUNTEST(&gadgetTOC_seekToDescriptor_test, hasFailed);
		RUNTEST(&gadgetTOC_isValid_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for gadgetHeader:\n");
		RUNTEST(&gadgetHeader_new_test, hasFailed);
		RUNTEST(&gadgetHeader_del_test, hasFailed);
		RUNTEST(&gadgetHeader_setNall_test, hasFailed);
		RUNTEST(&gadgetHeader_getNumPartsInFileWithMass_test, hasFailed);
		RUNTEST(&gadgetHeader_getNumPartsInBlock_test, hasFailed);
		RUNTEST(&gadgetHeader_sizeOfElement_test, hasFailed);
	}

	if (rank == 0) {
		printf("\nRunning tests for gadget:\n");
		RUNTEST(&gadget_new_test, hasFailed);
		RUNTEST(&gadget_newSimple_test, hasFailed);
		RUNTEST(&gadget_del_test, hasFailed);
		RUNTEST(&gadget_writeHeaderToCurrentFile_test, hasFailed);
		RUNTEST(&gadget_writeBlockToCurrentFile_test, hasFailed);
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

	if (rank == 0)
		printf("\nRunning tests for groupi:\n");
	RUNTESTMPI(&groupi_new_test, hasFailed);
	RUNTESTMPI(&groupi_del_test, hasFailed);
	RUNTESTMPI(&groupi_test, hasFailed);

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

#ifdef WITH_MPI
static bool
local_runtestMPI(bool       (*f
                          )(void))
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
