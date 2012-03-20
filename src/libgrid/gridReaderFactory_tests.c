// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderFactory_tests.c
 * @ingroup libgridIOInFactoryTests
 * @brief  This file implements the testing routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderFactory.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/

extern bool
gridReaderFactory_newReaderFromIni_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	gridReader_t reader;
	parse_ini_t  ini;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	ini    = parse_ini_open("tests/reading.ini");

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = gridReaderFactory_newReaderFromIni(ini, "ReaderBov");
	if (strcmp(filename_getFullName(gridReader_getFileName(reader)),
	           "tests/test.bov") != 0)
		hasPassed = false;
	gridReader_del(&reader);

	reader = gridReaderFactory_newReaderFromIni(ini, "ReaderBov2");
	if (strcmp(filename_getFullName(gridReader_getFileName(reader)),
	           "tests/test2.bov") != 0)
		hasPassed = false;
	gridReader_del(&reader);

	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
