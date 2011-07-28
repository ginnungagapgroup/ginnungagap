// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/cubepm_tests.c
 * @ingroup  libutilFilesCubePM
 * @brief  Implements the tests for cubepm.c
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "cubepm_tests.h"
#include "cubepm.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef ENABLE_XMEM_TRACK_MEM
#  include "xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "cubepm_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
cubepm_new_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	cubepm_t cubepm;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	cubepm = cubepm_new("./", "test", 5, 3000);

	if (cubepm->nodesDim != 5)
		hasPassed = false;
	if (cubepm->ngrid != 3000)
		hasPassed = false;
	if (cubepm->numFiles != 5 * 5 * 5)
		hasPassed = false;
	if (strcmp(cubepm->fileNames[0], "./test0.dat") != 0)
		hasPassed = false;
	if (strcmp(cubepm->fileNames[33], "./test33.dat") != 0)
		hasPassed = false;
	if (strcmp(cubepm->fileNames[124], "./test124.dat") != 0)
		hasPassed = false;

	cubepm_del(&cubepm);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
cubepm_del_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	cubepm_t cubepm;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	cubepm = cubepm_new("./", "test", 5, 3000);
	cubepm_del(&cubepm);
	if (cubepm != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
