// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterHDF5_tests.c
 * @ingroup  libgridIOOutHDF5
 * @brief  Implements the tests for gridWriterHDF5.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterHDF5_tests.h"
#include "gridWriterHDF5.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "gridWriterHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
gridWriterHDF5_new_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterHDF5_t gridWriterHDF5;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridWriterHDF5 = gridWriterHDF5_new();
	gridWriterHDF5_del((gridWriter_t)&gridWriterHDF5);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridWriterHDF5_del_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterHDF5_t gridWriterHDF5;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	gridWriterHDF5 = gridWriterHDF5_new();
	gridWriterHDF5_del((gridWriter_t)&gridWriterHDF5);
	if (gridWriterHDF5 != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
