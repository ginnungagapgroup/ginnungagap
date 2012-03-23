// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetVersion_tests.c
 * @ingroup  libutilFilesGadgetVersionTests
 * @brief  Provides the implementation of the tests functions for
 *         gadgetVersion.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetVersion_tests.h"
#include "gadgetVersion.h"
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
gadgetVersion_getVersionFromFile_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	FILE *f;
	gadgetVersion_t ver;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	f = xfopen("tests/gadgetFake_v1.little.dat", "rb");
	ver = gadgetVersion_getVersionFromFile(f);
	xfclose(&f);
	if (ver != GADGETVERSION_ONE)
		hasPassed = false;

	f = xfopen("tests/gadgetFake_v1.big.dat", "rb");
	ver = gadgetVersion_getVersionFromFile(f);
	xfclose(&f);
	if (ver != GADGETVERSION_ONE)
		hasPassed = false;

	f = xfopen("tests/gadgetFake_v2.little.dat", "rb");
	ver = gadgetVersion_getVersionFromFile(f);
	xfclose(&f);
	if (ver != GADGETVERSION_TWO)
		hasPassed = false;

	f = xfopen("tests/gadgetFake_v2.big.dat", "rb");
	ver = gadgetVersion_getVersionFromFile(f);
	xfclose(&f);
	if (ver != GADGETVERSION_TWO)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gadgetVersion_getNameFromType_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (strcmp(gadgetVersion_getNameFromType(GADGETVERSION_UNKNOWN),
	           "Unknown") != 0)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}


/*--- Implementations of local functions --------------------------------*/
