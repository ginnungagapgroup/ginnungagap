// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetBlock_tests.c
 * @ingroup  libutilFilesGadgetBlockTests
 * @brief  Provides the implementation of the tests functions for
 *         gadgetBlock.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetBlock_tests.h"
#include "gadgetBlock.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xfile.h"
#ifdef ENABLE_XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
gadgetBlock_getNameFromType_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (strcmp(gadgetBlock_getNameFromType(GADGETBLOCK_HEAD),
	           "HEAD") != 0)
		hasPassed = false;

	if (strcmp(gadgetBlock_getNameFromType(GADGETBLOCK_ID__),
	           "ID  ") != 0)
		hasPassed = false;

	if (strcmp(gadgetBlock_getNameFromType(GADGETBLOCK_HSML),
	           "HSML") != 0)
		hasPassed = false;

	if (strcmp(gadgetBlock_getNameFromType(GADGETBLOCK_POS_),
	           "POS ") != 0)
		hasPassed = false;

	if (strcmp(gadgetBlock_getNameFromType(GADGETBLOCK_VEL_),
	           "VEL ") != 0)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetBlock_getNameFromType_test */

extern bool
gadgetBlock_getTypeFromName_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (gadgetBlock_getTypeFromName("HEAD") != GADGETBLOCK_HEAD)
		hasPassed = false;

	if (gadgetBlock_getTypeFromName("ID  ") != GADGETBLOCK_ID__)
		hasPassed = false;

	if (gadgetBlock_getTypeFromName("MASS") != GADGETBLOCK_MASS)
		hasPassed = false;

	if (gadgetBlock_getTypeFromName("AGE ") != GADGETBLOCK_AGE_)
		hasPassed = false;

	if (gadgetBlock_getTypeFromName("VEL ") != GADGETBLOCK_VEL_)
		hasPassed = false;

	if (gadgetBlock_getTypeFromName("NE  ") != GADGETBLOCK_NE__)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetBlock_getTypeFromName_test */

extern bool
gadgetBlock_getNumComponents_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (gadgetBlock_getNumComponents(GADGETBLOCK_ID__) != 1)
		hasPassed = false;

	if (gadgetBlock_getNumComponents(GADGETBLOCK_HSML) != 1)
		hasPassed = false;

	if (gadgetBlock_getNumComponents(GADGETBLOCK_VEL_) != 3)
		hasPassed = false;

	if (gadgetBlock_getNumComponents(GADGETBLOCK_POS_) != 3)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetBlock_isInteger_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (gadgetBlock_isInteger(GADGETBLOCK_POS_))
		hasPassed = false;

	if (!gadgetBlock_isInteger(GADGETBLOCK_ID__))
		hasPassed = false;

	if (gadgetBlock_isInteger(GADGETBLOCK_MASS))
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetBlock_getNumPartsInBlock_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	uint32_t np[6]      = {1, 2, 4, 8, 16, 32};
	double   massarr[6] = {1.0, 0.0, 0.0, 1.0, 0.0, 1.0};

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (gadgetBlock_getNumPartsInBlock(GADGETBLOCK_POS_, np, massarr) != 63)
		hasPassed = false;

	if (gadgetBlock_getNumPartsInBlock(GADGETBLOCK_ID__, np, massarr) != 63)
		hasPassed = false;

	if (gadgetBlock_getNumPartsInBlock(GADGETBLOCK_MASS, np, massarr) != 22)
		hasPassed = false;

	if (gadgetBlock_getNumPartsInBlock(GADGETBLOCK_AGE_, np, massarr) != 16)
		hasPassed = false;

	if (gadgetBlock_getNumPartsInBlock(GADGETBLOCK_SFR_, np, massarr) != 1)
		hasPassed = false;

	if (gadgetBlock_getNumPartsInBlock(GADGETBLOCK_Z___, np, massarr) != 17)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetBlock_getNumPartsInBlock_test */

extern bool
gadgetBlock_writereadDescriptor_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0) {
		printf("Testing %s... ", __func__);

		FILE          *f     = xfopen("TEST_gadgetBlock.dat", "w+b");
		long          oldPos = xftell(f);
		gadgetBlock_t blockRead;
		uint32_t      sizeRead;
		gadgetBlock_writeDescriptor(f, GADGETBLOCK_MASS, 64, false,
		                            GADGETVERSION_ONE);
		if (oldPos != xftell(f))
			hasPassed = false;

		gadgetBlock_writeDescriptor(f, GADGETBLOCK_MASS, 64, false,
		                            GADGETVERSION_TWO);
		if (oldPos + 16L != xftell(f))
			hasPassed = false;

		xfseek(f, -16, SEEK_CUR);

		gadgetBlock_readDescriptor(f, &blockRead, &sizeRead, false,
		                           GADGETVERSION_ONE);
		if (oldPos != xftell(f))
			hasPassed = false;

		gadgetBlock_readDescriptor(f, &blockRead, &sizeRead, false,
		                           GADGETVERSION_TWO);
		if (oldPos + 16L != xftell(f))
			hasPassed = false;
		if (blockRead != GADGETBLOCK_MASS)
			hasPassed = false;
		if (sizeRead != 64)
			hasPassed = false;

		xfseek(f, -16, SEEK_CUR);
		gadgetBlock_writeDescriptor(f, GADGETBLOCK_ID__, 1313, true,
		                            GADGETVERSION_TWO);
		if (oldPos + 16L != xftell(f))
			hasPassed = false;
		xfseek(f, -16, SEEK_CUR);
		gadgetBlock_readDescriptor(f, &blockRead, &sizeRead, true,
		                           GADGETVERSION_TWO);
		if (oldPos + 16L != xftell(f))
			hasPassed = false;
		if (blockRead != GADGETBLOCK_ID__)
			hasPassed = false;
		if (sizeRead != 1313)
			hasPassed = false;


		xfclose(&f);
#ifdef XMEM_TRACK_MEM
		if (allocatedBytes != global_allocated_bytes)
			hasPassed = false;
#endif
	}

	return hasPassed ? true : false;
} /* gadgetBlock_writeDescriptor_test */

extern bool
gadgetBlock_readDescriptorString_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0) {
		printf("Testing %s... ", __func__);

		FILE     *f      = xfopen("TEST_gadgetBlock.dat", "rb");
		long     oldPos  = xftell(f);
		char     name[4] = "XXXX";
		uint32_t bs      = 0;

		gadgetBlock_readDescriptorString(f, name, &bs, true,
		                                 GADGETVERSION_ONE);
		if (oldPos != xftell(f))
			hasPassed = false;
		if (strncmp(name, "XXXX", 4) != 0)
			hasPassed = false;
		if (bs != 0)
			hasPassed = false;

		gadgetBlock_readDescriptorString(f, name, &bs, true,
		                                 GADGETVERSION_TWO);
		if (oldPos + 16L != xftell(f))
			hasPassed = false;
		if (strncmp(name, "ID  ", 4) != 0)
			hasPassed = false;
		if (bs != 1313)
			hasPassed = false;

		xfclose(&f);
#ifdef XMEM_TRACK_MEM
		if (allocatedBytes != global_allocated_bytes)
			hasPassed = false;
#endif
	}

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
