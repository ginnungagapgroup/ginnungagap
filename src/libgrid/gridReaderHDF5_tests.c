// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5_tests.c
 * @ingroup  libgridIOInHDF5
 * @brief  Implements the test for gridReaderHDF5.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderHDF5_tests.h"
#include "gridReaderHDF5.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "gridReaderHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
gridReaderHDF5_newFromIni_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridReaderHDF5_t gridReaderHDF5;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);
#if 0
	gridReaderHDF5 = gridReaderHDF5_new();
	gridReaderHDF5_del(&gridReaderHDF5);
#endif
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderHDF5_del_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridReaderHDF5_t gridReaderHDF5;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

#if 0
	gridReaderHDF5 = gridReaderHDF5_new();
	gridReaderHDF5_del(&gridReaderHDF5);
#endif
	if (gridReaderHDF5 != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderHDF5_readIntoPatch_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridReaderHDF5_t gridReaderHDF5;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

#if 0
	gridReaderHDF5 = gridReaderHDF5_new();
	gridReaderHDF5_del(&gridReaderHDF5);
#endif
	if (gridReaderHDF5 != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderHDF5_readIntoPatchForVar_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridReaderHDF5_t gridReaderHDF5;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

#if 0
	gridReaderHDF5 = gridReaderHDF5_new();
	gridReaderHDF5_del(&gridReaderHDF5);
#endif
	if (gridReaderHDF5 != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
