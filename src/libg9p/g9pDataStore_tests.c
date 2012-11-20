// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pDataStore_tests.c
 * @ingroup  libg9pDatastore
 * @brief  Implements the test for the data store.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pDataStore_tests.h"
#include "g9pDataStore.h"
#include "g9pHierarchy.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif
#include "../libutil/filename.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pDataStore_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static g9pHierarchy_t
local_getHierarchy(void);


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pDataStore_verifyCreation(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pDataStore_t ds;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ds = g9pDataStore_new(local_getHierarchy(), "test", "../data/");
	g9pDataStore_del(&ds);
	ds = g9pDataStore_new(local_getHierarchy(), "test", NULL);
	g9pDataStore_del(&ds);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
g9pDataStore_verifyFileNames(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pDataStore_t ds;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ds = g9pDataStore_new(local_getHierarchy(), "test", "../data");
	filename_t fn = g9pDataStore_getFileName(ds, 0, 0);
	if (strcmp(filename_getFullName(fn), "../data/00_32/test_wn") != 0)
		hasPassed = false;
	filename_del(&fn);

	fn = g9pDataStore_getFileName(ds, 5, 2);
	if (strcmp(filename_getFullName(fn), "../data/05_7680/test_velx") != 0)
		hasPassed = false;
	filename_del(&fn);

	g9pDataStore_del(&ds);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
static g9pHierarchy_t
local_getHierarchy(void)
{
	const static uint32_t factors[6] = { 2, 2, 3, 5, 4 };
	return g9pHierarchy_newWithExplicitFactors(6, 32, factors);
}
