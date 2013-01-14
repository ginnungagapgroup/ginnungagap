// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetHeader_tests.c
 * @ingroup  libutilFilesGadgetHeaderTests
 * @brief  Provides the implementation of the tests functions for
 *         gadgetHeader.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetHeader_tests.h"
#include "gadgetHeader.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xfile.h"
#ifdef ENABLE_XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "gadgetHeader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
gadgetHeader_new_test(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	gadgetHeader_t header;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	header = gadgetHeader_new();

	for (int i = 0; i < 6; i++) {
		if (header->np[i] != 0)
			hasPassed = false;
		if (header->nall[i] != 0)
			hasPassed = false;
		if (header->nallhighw[i] != 0)
			hasPassed = false;
		if (islessgreater(header->massarr[i], 0.0))
			hasPassed = false;
	}
	if (header->useLongIDs)
		hasPassed = false;

	gadgetHeader_del(&header);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetHeader_new_test */

extern bool
gadgetHeader_del_test(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	gadgetHeader_t header;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	header = gadgetHeader_new();
	gadgetHeader_del(&header);
	if (header != NULL)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetHeader_setNall_test(void)
{
	bool           hasPassed      = true;
	int            rank           = 0;
	gadgetHeader_t header;
	uint64_t       nall[6]        = {1000, 1000, 0, 0, 0, 0};
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	header = gadgetHeader_new();

	gadgetHeader_setNall(header, nall);
	if (gadgetHeader_getTotalNumParts(header) != nall[0] + nall[1])
		hasPassed = false;
	if (header->useLongIDs)
		hasPassed = false;

	nall[0] = 1L << 36;
	nall[1] = 1L << 31;
	nall[4] = 1L << 32;
	gadgetHeader_setNall(header, nall);
	if (gadgetHeader_getTotalNumParts(header) != nall[0] + nall[1]
	    + nall[4])
		hasPassed = false;
	if (!header->useLongIDs)
		hasPassed = false;

	gadgetHeader_del(&header);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadgetHeader_setNall_test */

extern bool
gadgetHeader_getNumPartsInFileWithMass_test(void)
{
	bool           hasPassed      = true;
	int            rank           = 0;
	gadgetHeader_t header;
	uint32_t       np[6]          = {1000, 1000, 0, 0, 0, 0};
	double         massarr[6]     = {1., 0., 0., 0., 0., 0.};
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	header = gadgetHeader_new();
	gadgetHeader_setNp(header, np);
	gadgetHeader_setMassArr(header, massarr);

	if (gadgetHeader_getNumPartsInFileWithMass(header) != 1000)
		hasPassed = false;

	gadgetHeader_del(&header);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetHeader_getNumPartsInBlock_test(void)
{
	bool           hasPassed      = true;
	int            rank           = 0;
	gadgetHeader_t header;
	uint32_t       np[6]          = {1000, 1000, 0, 0, 12, 0};
	double         massarr[6]     = {1., 0., 0., 0., 0., 0.};
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	header = gadgetHeader_new();
	gadgetHeader_setNp(header, np);
	gadgetHeader_setMassArr(header, massarr);

	if (gadgetHeader_getNumPartsInBlock(header, GADGETBLOCK_POS_) != 2012)
		hasPassed = false;
	if (gadgetHeader_getNumPartsInBlock(header, GADGETBLOCK_MASS) != 1012)
		hasPassed = false;
	if (gadgetHeader_getNumPartsInBlock(header, GADGETBLOCK_Z___) != 1012)
		hasPassed = false;
	if (gadgetHeader_getNumPartsInBlock(header, GADGETBLOCK_SFR_) != 1000)
		hasPassed = false;

	gadgetHeader_del(&header);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetHeader_sizeOfElement_test(void)
{
	bool           hasPassed      = true;
	int            rank           = 0;
	gadgetHeader_t header;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	header = gadgetHeader_new();

	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_POS_) != 12)
		hasPassed = false;
	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_ID__) != 4)
		hasPassed = false;

	gadgetHeader_setFlagDoublePrecision(header, 1);
	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_POS_) != 24)
		hasPassed = false;
	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_ID__) != 4)
		hasPassed = false;

	gadgetHeader_setFlagDoublePrecision(header, 0);
	gadgetHeader_setUseLongIDs(header, true);
	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_POS_) != 12)
		hasPassed = false;
	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_ID__) != 8)
		hasPassed = false;

	gadgetHeader_setFlagDoublePrecision(header, 1);
	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_POS_) != 24)
		hasPassed = false;
	if (gadgetHeader_sizeOfElement(header, GADGETBLOCK_ID__) != 8)
		hasPassed = false;

	gadgetHeader_del(&header);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}


/*--- Implementations of local functions --------------------------------*/
