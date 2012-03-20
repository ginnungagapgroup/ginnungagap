// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReader_tests.c
 * @ingroup libgridIOInInterfaceTests
 * @brief  This file implements the testing routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_tests.h"
#include "gridReader.h"
#include "gridReaderFactory.h"
#include <stdio.h>
#include <string.h>
#include "../libutil/filename.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridReader_t
local_getReader(void);


/*--- Implementations of exported functios ------------------------------*/

extern bool
gridReader_setFileName_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	gridReader_t reader;
	filename_t   newName;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader  = local_getReader();

	newName = filename_newFull("tests/", "test2", NULL, ".bov");
	gridReader_setFileName(reader, newName);
	if (strcmp(filename_getFullName(reader->fileName),
	           "tests/test2.bov") != 0)
		hasPassed = false;

	gridReader_del(&reader);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReader_overlayFileName_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	gridReader_t reader;
	filename_t   overlayName;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader      = local_getReader();

	overlayName = filename_newFull(NULL, "test2", NULL, NULL);
	gridReader_overlayFileName(reader, overlayName);
	if (strcmp(filename_getFullName(reader->fileName),
	           "tests/test2.bov") != 0)
		hasPassed = false;

	filename_del(&overlayName);
	gridReader_del(&reader);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReader_getFileName_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridReader_t     reader;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader   = local_getReader();

	const filename_t fileName = gridReader_getFileName(reader);

	if (fileName != reader->fileName)
		hasPassed = false;

	gridReader_del(&reader);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
static gridReader_t
local_getReader(void)
{
	gridReader_t reader;
	parse_ini_t  ini;

	ini    = parse_ini_open("tests/reading.ini");
	reader = gridReaderFactory_newReaderFromIni(ini, "ReaderBov");
	parse_ini_close(&ini);

	return reader;
}
