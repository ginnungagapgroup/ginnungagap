// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridIO_tests.h"
#include "gridIO.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridIO_getTypeFromName_test(void)
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

	if (gridIO_getTypeFromName("bov") != IO_TYPE_BOV)
		hasPassed = false;
	if (gridIO_getTypeFromName("silo") != IO_TYPE_SILO)
		hasPassed = false;
	if (gridIO_getTypeFromName("This will fail") != IO_TYPE_UNKNOWN)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridIO_getNameFromType_test(void)
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

	if (strcmp("bov", gridIO_getNameFromType(IO_TYPE_BOV)) != 0)
		hasPassed = false;
	if (strcmp("silo", gridIO_getNameFromType(IO_TYPE_SILO)) != 0)
		hasPassed = false;
	if (strcmp("unknown", gridIO_getNameFromType(IO_TYPE_UNKNOWN)) != 0)
		hasPassed = false;
	if (strcmp("unknown",
	           gridIO_getNameFromType(IO_TYPE_UNKNOWN * 14)) != 0)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridIO_getNameFromType_test */

/*--- Implementations of local functions --------------------------------*/
