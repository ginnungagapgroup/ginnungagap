// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadget_tests.c
 * @ingroup  libutilFilesGadgetTests
 * @brief  Provides the implementation of the tests functions for
 *         gadget.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadget_tests.h"
#include "gadget.h"
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
#include "gadget_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gadget_t
local_getGadgetSimpleRead(void);

static gadget_t
local_getGadgetSimpleWrite(void);


/*--- Implementations of exported functions -----------------------------*/
extern bool
gadget_new_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	gadget_t gadget;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gadget = gadget_new();

	gadget_del(&gadget);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadget_newSimple_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	gadget_t gadget;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gadget = gadget_newSimple("test.dat", 1);
	if (gadget_getNumFiles(gadget) != 1)
		hasPassed = false;
	if (gadget_getFileVersion(gadget) != GADGETVERSION_TWO)
		hasPassed = false;
	if (strcmp(gadget_getFileNameOfFile(gadget, 0), "test.dat") != 0)
		hasPassed = false;
	gadget_del(&gadget);

	gadget = gadget_newSimple("test.dat", 16);
	if (gadget_getNumFiles(gadget) != 16)
		hasPassed = false;
	if (strcmp(gadget_getFileNameOfFile(gadget, 0), "test.dat.0") != 0)
		hasPassed = false;
	if (strcmp(gadget_getFileNameOfFile(gadget, 5), "test.dat.5") != 0)
		hasPassed = false;
	if (strcmp(gadget_getFileNameOfFile(gadget, 15), "test.dat.15") != 0)
		hasPassed = false;
	gadget_del(&gadget);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadget_newSimple_test */

extern bool
gadget_del_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	gadget_t gadget;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gadget = local_getGadgetSimpleRead();
	gadget_del(&gadget);
	if (gadget != NULL)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadget_writeHeaderToCurrentFile_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	gadget_t gadget;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gadget = local_getGadgetSimpleWrite();

	gadget_open(gadget, GADGET_MODE_WRITE_CREATE, 0);
	gadget_writeHeaderToCurrentFile(gadget);
	gadget_close(gadget);

	gadget_del(&gadget);
	if (gadget != NULL)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadget_writeBlockToCurrentFile_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	gadget_t gadget;
	stai_t   stai;
	float    data[20][3] = {{1.f,  1.f,  1.f }, {2.f,  2.f,  2.f },
							{3.f,  3.f,  3.f }, {4.f,  4.f,  4.f },
							{5.f,  5.f,  5.f }, {6.f,  6.f,  6.f },
							{7.f,  7.f,  7.f }, {8.f,  8.f,  8.f },
							{9.f,  9.f,  9.f }, {10.f, 10.f, 10.f},
							{11.f, 11.f, 11.f}, {12.f, 12.f, 12.f},
							{13.f, 13.f, 13.f}, {14.f, 14.f, 14.f},
							{15.f, 15.f, 15.f}, {16.f, 16.f, 16.f},
							{17.f, 17.f, 17.f}, {18.f, 18.f, 18.f},
							{19.f, 19.f, 19.f}, {20.f, 20.f, 20.f}};
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gadget = local_getGadgetSimpleWrite();

	gadget_setFileNamesFromStem(gadget, "gadgetFake_v1.little.2.dat");
	gadget_setFileVersion(gadget, GADGETVERSION_ONE);
	gadget_setFileEndianess(gadget, ENDIAN_LITTLE);
	gadget_createEmptyFile(gadget, 0);
	gadget_open(gadget, GADGET_MODE_WRITE_CONT, 0);
	gadget_writeHeaderToCurrentFile(gadget);
	stai = stai_new(&(data[0][0]), 3 * sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_POS_, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_VEL_, 0, 20, stai);
	stai_del(&stai);
	stai = stai_new(&(data[0][0]), sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_ID__, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_MASS, 0, 10, stai);
	stai_del(&stai);
	gadget_close(gadget);
	if (!xfile_filesAreEqual("gadgetFake_v1.little.2.dat",
	                         "tests/gadgetFake_v1.little.dat"))
		hasPassed = false;

	gadget_setFileNamesFromStem(gadget, "gadgetFake_v1.big.2.dat");
	gadget_setFileVersion(gadget, GADGETVERSION_ONE);
	gadget_setFileEndianess(gadget, ENDIAN_BIG);
	gadget_createEmptyFile(gadget, 0);
	gadget_open(gadget, GADGET_MODE_WRITE_CONT, 0);
	gadget_writeHeaderToCurrentFile(gadget);
	stai = stai_new(&(data[0][0]), 3 * sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_POS_, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_VEL_, 0, 20, stai);
	stai_del(&stai);
	stai = stai_new(&(data[0][0]), sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_ID__, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_MASS, 0, 10, stai);
	stai_del(&stai);
	gadget_close(gadget);
	if (!xfile_filesAreEqual("gadgetFake_v1.big.2.dat",
	                         "tests/gadgetFake_v1.big.dat"))
		hasPassed = false;

	gadget_setFileNamesFromStem(gadget, "gadgetFake_v2.little.2.dat");
	gadget_setFileVersion(gadget, GADGETVERSION_TWO);
	gadget_setFileEndianess(gadget, ENDIAN_LITTLE);
	gadget_createEmptyFile(gadget, 0);
	gadget_open(gadget, GADGET_MODE_WRITE_CONT, 0);
	gadget_writeHeaderToCurrentFile(gadget);
	stai = stai_new(&(data[0][0]), 3 * sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_POS_, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_VEL_, 0, 20, stai);
	stai_del(&stai);
	stai = stai_new(&(data[0][0]), sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_ID__, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_MASS, 0, 10, stai);
	stai_del(&stai);
	gadget_close(gadget);
	if (!xfile_filesAreEqual("gadgetFake_v2.little.2.dat",
	                         "tests/gadgetFake_v2.little.dat"))
		hasPassed = false;

	gadget_setFileNamesFromStem(gadget, "gadgetFake_v2.big.2.dat");
	gadget_setFileVersion(gadget, GADGETVERSION_TWO);
	gadget_setFileEndianess(gadget, ENDIAN_LITTLE);
	gadget_createEmptyFile(gadget, 0);
	gadget_open(gadget, GADGET_MODE_WRITE_CONT, 0);
	gadget_writeHeaderToCurrentFile(gadget);
	stai = stai_new(&(data[0][0]), 3 * sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_POS_, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_VEL_, 0, 20, stai);
	stai_del(&stai);
	stai = stai_new(&(data[0][0]), sizeof(float), 3 * sizeof(float));
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_ID__, 0, 20, stai);
	gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_MASS, 0, 10, stai);
	stai_del(&stai);
	gadget_close(gadget);
	if (!xfile_filesAreEqual("gadgetFake_v2.big.2.dat",
	                         "tests/gadgetFake_v2.big.dat"))
		hasPassed = false;

	gadget_del(&gadget);
	if (gadget != NULL)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gadget_writeBlockToCurrentFile_test */

/*--- Implementations of local functions --------------------------------*/
static gadget_t
local_getGadgetSimpleRead(void)
{
	gadget_t gadget = gadget_newSimple("tests/gadgetFake_v2.big.dat", 1);

	gadget_initForRead(gadget);

	return gadget;
}

static gadget_t
local_getGadgetSimpleWrite(void)
{
	gadget_t gadget = gadget_newSimple("TEST_gadget_writing.dat", 1);

	// Header
	uint32_t       np[6]      = {10, 10, 0, 0, 0, 0};
	uint64_t       nall[6]    = {10L, 10L, 0L, 0L, 0L, 0L};
	double         massarr[6] = {0.0, 1.0, 0.0, 0.0, 0.0, 0.0};
	gadgetHeader_t header     = gadgetHeader_new();
	gadgetHeader_setNp(header, np);
	gadgetHeader_setMassArr(header, massarr);
	gadgetHeader_setNall(header, nall);
	gadgetHeader_setNumFiles(header, gadget_getNumFiles(gadget));
	gadget_setHeaderOfFile(gadget, 0, header);

	// TOC
	gadgetTOC_t toc = gadgetTOC_new();
	gadgetTOC_setFileVersion(toc, gadget_getFileVersion(gadget));
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_HEAD);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_POS_);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_VEL_);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_ID__);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_MASS);
	gadgetTOC_calcSizes(toc, np, massarr,
	                    gadgetHeader_getFlagDoublePrecision(header),
	                    gadgetHeader_getUseLongIDs(header));
	gadgetTOC_calcOffset(toc);
	gadget_setTOCOfFile(gadget, 0, toc);

	// Done
	return gadget;
}
