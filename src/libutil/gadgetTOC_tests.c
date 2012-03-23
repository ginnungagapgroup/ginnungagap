// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetTOC_tests.c
 * @ingroup  libutilFilesGadgetTOCTests
 * @brief  Provides the implementation of the tests functions for
 *         gadgetTOC.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetTOC_tests.h"
#include "gadgetTOC.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xfile.h"
#ifdef ENABLE_XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "gadgetTOC_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gadgetTOC_t
local_getBasicTOC(void);

static void
local_initBasicTOC(gadgetTOC_t toc);


/*--- Implementations of exported functions -----------------------------*/
extern bool
gadgetTOC_new_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = gadgetTOC_new();
	if (toc->fileVersion != GADGETVERSION_TWO)
		hasPassed = false;
	if (toc->numBlocks != 0)
		hasPassed = false;
	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_newFromFile_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	FILE *f = xfopen("tests/gadgetFake_v2.big.dat", "rb");
	toc = gadgetTOC_newFromFile(f);
	xfclose(&f);

	if (toc->fileVersion != GADGETVERSION_TWO)
		hasPassed = false;
	if (toc->numBlocks != 5)
		hasPassed = false;
	if (toc->blocks[0].offset != 16L)
		hasPassed = false;
	if (toc->blocks[3].offset != 824L)
		hasPassed = false;

	gadgetTOC_del(&toc);
	if (toc != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetTOC_newFromFile_test */

extern bool
gadgetTOC_clone_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc, clone;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	clone = gadgetTOC_clone(toc);

	if (clone->fileVersion != toc->fileVersion)
		hasPassed = false;
	if (clone->numBlocks != toc->numBlocks)
		hasPassed = false;
	if (clone->blocks == toc->blocks)
		hasPassed = false;
	if (clone->blocks[0].offset != toc->blocks[0].offset)
		hasPassed = false;
	if (clone->blocks[0].type != toc->blocks[0].type)
		hasPassed = false;
	if (clone->blocks[0].sizeInBytes != toc->blocks[0].sizeInBytes)
		hasPassed = false;
	if (strcmp(clone->blocks[0].nameInV2Files,
	           toc->blocks[0].nameInV2Files) != 0)
		hasPassed = false;

	gadgetTOC_del(&clone);
	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}


extern bool
gadgetTOC_del_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	gadgetTOC_del(&toc);
	if (toc != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_setFileVersion_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = gadgetTOC_new();

	if (toc->fileVersion != GADGETVERSION_TWO)
		hasPassed = false;
	gadgetTOC_setFileVersion(toc, GADGETVERSION_ONE);
	if (toc->fileVersion != GADGETVERSION_ONE)
		hasPassed = false;
	gadgetTOC_setFileVersion(toc, GADGETVERSION_TWO);
	if (toc->fileVersion != GADGETVERSION_TWO)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_getFileVersion_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = gadgetTOC_new();

	gadgetTOC_setFileVersion(toc, GADGETVERSION_ONE);
	if (GADGETVERSION_ONE != gadgetTOC_getFileVersion(toc))
		hasPassed = false;
	gadgetTOC_setFileVersion(toc, gadgetTOC_getFileVersion(toc));
	if (GADGETVERSION_TWO != GADGETVERSION_TWO)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_blockExists_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (!gadgetTOC_blockExists(toc, GADGETBLOCK_POS_))
		hasPassed = false;
	if (!gadgetTOC_blockExists(toc, GADGETBLOCK_VEL_))
		hasPassed = false;
	if (!gadgetTOC_blockExists(toc, GADGETBLOCK_ID__))
		hasPassed = false;
	if (gadgetTOC_blockExists(toc, GADGETBLOCK_U___))
		hasPassed = false;
	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_blockExistsByName_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (!gadgetTOC_blockExistsByName(toc, "POS "))
		hasPassed = false;
	if (!gadgetTOC_blockExistsByName(toc, "VEL "))
		hasPassed = false;
	if (!gadgetTOC_blockExistsByName(toc, "ID  "))
		hasPassed = false;
	if (gadgetTOC_blockExistsByName(toc, "U   "))
		hasPassed = false;
	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_getOffsetForBlock_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (gadgetTOC_getOffsetForBlock(toc, GADGETBLOCK_POS_) != -1L)
		hasPassed = false;
	local_initBasicTOC(toc);
	if (gadgetTOC_getOffsetForBlock(toc, GADGETBLOCK_HEAD) != 16L)
		hasPassed = false;
	if (gadgetTOC_getOffsetForBlock(toc, GADGETBLOCK_VEL_) != 524L)
		hasPassed = false;
	if (gadgetTOC_getOffsetForBlock(toc, GADGETBLOCK_MASS) != 752L)
		hasPassed = false;
	if (gadgetTOC_getOffsetForBlock(toc, GADGETBLOCK_ID__) != 784L)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetTOC_getOffsetForBlock_test */

extern bool
gadgetTOC_getSizeInBytesForBlock_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (gadgetTOC_getSizeInBytesForBlock(toc, GADGETBLOCK_POS_) != 0)
		hasPassed = false;
	local_initBasicTOC(toc);
	if (gadgetTOC_getSizeInBytesForBlock(toc, GADGETBLOCK_HEAD) != 256)
		hasPassed = false;
	if (gadgetTOC_getSizeInBytesForBlock(toc, GADGETBLOCK_VEL_) != 204)
		hasPassed = false;
	if (gadgetTOC_getSizeInBytesForBlock(toc, GADGETBLOCK_MASS) != 8)
		hasPassed = false;
	if (gadgetTOC_getSizeInBytesForBlock(toc, GADGETBLOCK_ID__) != 136)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetTOC_getSizeInBytesForBlock_test */

extern bool
gadgetTOC_getNameInV2FilesForBlock_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (strncmp(gadgetTOC_getNameInV2FilesForBlock(toc, GADGETBLOCK_POS_),
	            "POS ", 4) != 0)
		hasPassed = false;
	if (strncmp(gadgetTOC_getNameInV2FilesForBlock(toc, GADGETBLOCK_MASS),
	            "MASS", 4) != 0)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_getOffsetBySeqNumber_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (gadgetTOC_getOffsetBySeqNumber(toc, 1) != -1L)
		hasPassed = false;
	local_initBasicTOC(toc);
	if (gadgetTOC_getOffsetBySeqNumber(toc, 0) != 16L)
		hasPassed = false;
	if (gadgetTOC_getOffsetBySeqNumber(toc, 2) != 524L)
		hasPassed = false;
	if (gadgetTOC_getOffsetBySeqNumber(toc, 3) != 752L)
		hasPassed = false;
	if (gadgetTOC_getOffsetBySeqNumber(toc, 4) != 784L)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_getTypeBySeqNumber_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (gadgetTOC_getTypeBySeqNumber(toc, 0) != GADGETBLOCK_HEAD)
		hasPassed = false;
	if (gadgetTOC_getTypeBySeqNumber(toc, 1) != GADGETBLOCK_POS_)
		hasPassed = false;
	if (gadgetTOC_getTypeBySeqNumber(toc, 4) != GADGETBLOCK_ID__)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_getSizeInBytesBySeqNumber_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (gadgetTOC_getSizeInBytesBySeqNumber(toc, 1) != 0)
		hasPassed = false;
	local_initBasicTOC(toc);
	if (gadgetTOC_getSizeInBytesBySeqNumber(toc, 0) != 256)
		hasPassed = false;
	if (gadgetTOC_getSizeInBytesBySeqNumber(toc, 2) != 204)
		hasPassed = false;
	if (gadgetTOC_getSizeInBytesBySeqNumber(toc, 3) != 8)
		hasPassed = false;
	if (gadgetTOC_getSizeInBytesBySeqNumber(toc, 4) != 136)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} 

extern bool
gadgetTOC_getNameBySeqNumber_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (strncmp(gadgetTOC_getNameBySeqNumber(toc, 1),
	            "POS ", 4) != 0)
		hasPassed = false;
	if (strncmp(gadgetTOC_getNameBySeqNumber(toc, 3),
	            "MASS", 4) != 0)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_getTotalFileSize_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	local_initBasicTOC(toc);

	if (gadgetTOC_getTotalFileSize(toc) != 928)

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_addEntryByName_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = gadgetTOC_new();

	gadgetTOC_addEntryByName(toc, "HEAD");
	if (toc->numBlocks != 1)
		hasPassed = false;
	if (toc->blocks[0].type != GADGETBLOCK_HEAD)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_addEntryByType_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = gadgetTOC_new();

	gadgetTOC_addEntryByType(toc, GADGETBLOCK_HEAD);
	if (toc->numBlocks != 1)
		hasPassed = false;
	if (strncmp(toc->blocks[0].nameInV2Files, "HEAD", 4) != 0)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_calcSizes_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();

	uint32_t np[6] = {0, 1, 0, 0, 0};
	double massarr[6] = {0., 1., 0., 0., 0., 0.};

	gadgetTOC_calcSizes(toc, np, massarr, false, false);
	if (toc->blocks[0].sizeInBytes != 256)
		hasPassed = false;
	if (toc->blocks[1].sizeInBytes != 12)
		hasPassed = false;
	if (toc->blocks[3].sizeInBytes != 0)
		hasPassed = false;
	if (toc->blocks[4].sizeInBytes != 4)
		hasPassed = false;

	np[0] = 1;
	gadgetTOC_calcSizes(toc, np, massarr, false, false);
	if (toc->blocks[1].sizeInBytes != 24)
		hasPassed = false;
	if (toc->blocks[3].sizeInBytes != 4)
		hasPassed = false;
	if (toc->blocks[4].sizeInBytes != 8)
		hasPassed = false;

	gadgetTOC_calcSizes(toc, np, massarr, true, false);
	if (toc->blocks[1].sizeInBytes != 48)
		hasPassed = false;
	if (toc->blocks[3].sizeInBytes != 8)
		hasPassed = false;
	if (toc->blocks[4].sizeInBytes != 8)
		hasPassed = false;

	gadgetTOC_calcSizes(toc, np, massarr, false, true);
	if (toc->blocks[1].sizeInBytes != 24)
		hasPassed = false;
	if (toc->blocks[3].sizeInBytes != 4)
		hasPassed = false;
	if (toc->blocks[4].sizeInBytes != 16)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_calcOffset_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	gadgetTOC_setFileVersion(toc, GADGETVERSION_TWO);
	gadgetTOC_calcOffset(toc);
	if (toc->blocks[0].offset != 16L)
		hasPassed = false;
	if (toc->blocks[1].offset != 40L)
		hasPassed = false;
	gadgetTOC_setFileVersion(toc, GADGETVERSION_ONE);
	gadgetTOC_calcOffset(toc);
	if (toc->blocks[0].offset != 0L)
		hasPassed = false;
	if (toc->blocks[1].offset != 8L)
		hasPassed = false;

	local_initBasicTOC(toc);
	if (toc->blocks[1].offset != 296L)
		hasPassed = false;
	if (toc->blocks[4].offset != 784L)
		hasPassed = false;

	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_seekToData_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
	uint32_t    bs;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	FILE *f = fopen("tests/gadgetFake_v2.little.dat", "rb");
	toc = gadgetTOC_newFromFile(f);

	gadgetTOC_seekToData(toc, GADGETBLOCK_HEAD, f);
	if (xftell(f) != 16L)
		hasPassed = false;
	xfread(&bs, sizeof(uint32_t), 1, f);
	if (bs != 256)
		hasPassed = false;

	fclose(f);
	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_seekToDescriptor_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
	uint32_t    bs;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	FILE *f = fopen("tests/gadgetFake_v2.little.dat", "rb");
	toc = gadgetTOC_newFromFile(f);

	gadgetTOC_seekToDescriptor(toc, GADGETBLOCK_POS_, f);
	if (xftell(f) != 280L)
		hasPassed = false;
	xfread(&bs, sizeof(uint32_t), 1, f);
	if (bs != 8)
		hasPassed = false;

	fclose(f);
	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetTOC_isValid_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	gadgetTOC_t toc;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	toc = local_getBasicTOC();
	if (gadgetTOC_isValid(toc))
		hasPassed = false;
	local_initBasicTOC(toc);
	if (!gadgetTOC_isValid(toc))
		hasPassed = false;
	gadgetTOC_del(&toc);

	FILE *f = fopen("tests/gadgetFake_v1.little.dat", "rb");
	toc = gadgetTOC_newFromFile(f);
	fclose(f);
	if (!gadgetTOC_isValid(toc))
		hasPassed = false;
	gadgetTOC_del(&toc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}


/*--- Implementations of local functions --------------------------------*/
static gadgetTOC_t
local_getBasicTOC(void)
{
	gadgetTOC_t toc = gadgetTOC_new();

	gadgetTOC_addEntryByName(toc, "HEAD");
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_POS_);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_VEL_);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_MASS);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_ID__);

	return toc;
}

static void
local_initBasicTOC(gadgetTOC_t toc)
{
	uint32_t np[6]      = {4, 5, 6, 0, 0, 2};
	double   massarr[6] = {1., 1., 1., 0., 0., 0.};

	gadgetTOC_calcSizes(toc, np, massarr, false, true);
	gadgetTOC_setFileVersion(toc, GADGETVERSION_TWO);
	gadgetTOC_calcOffset(toc);
}
