// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include "partDesc_tests.h"
#include "partDesc.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "partDesc_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_TEST_DESCNAME "testName1"
#define LOCAL_TEST_DESCID   45


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
partDesc_new_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	partDesc_t partDesc;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	partDesc = partDesc_new(LOCAL_TEST_DESCNAME, LOCAL_TEST_DESCID, 0);

	if (strcmp(partDesc->name, LOCAL_TEST_DESCNAME) != 0)
		hasPassed = false;
	if (partDesc->partDescID != LOCAL_TEST_DESCID)
		hasPassed = false;
	// The number of variables hint is not tested!

	partDesc_del(&partDesc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
partDesc_getRef_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	partDesc_t partDesc, partDescRef;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	partDesc    = partDesc_new(LOCAL_TEST_DESCNAME, LOCAL_TEST_DESCID, 0);
	partDescRef = partDesc_getRef(partDesc);

	if (strcmp(partDescRef->name, LOCAL_TEST_DESCNAME) != 0)
		hasPassed = false;
	if (partDescRef->partDescID != LOCAL_TEST_DESCID)
		hasPassed = false;
	// The number of variables hint is not tested!

	partDesc_del(&partDesc);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes == global_allocated_bytes)
		hasPassed = false;
#endif
	partDesc_del(&partDescRef);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
partDesc_del_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	partDesc_t partDesc;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	partDesc = partDesc_new(LOCAL_TEST_DESCNAME, LOCAL_TEST_DESCID, 0);
	partDesc_del(&partDesc);
	if (partDesc != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
