// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataParticle_tests.h"
#include "dataParticle.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "dataVar.h"
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "dataParticle_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_TEST_DESCNAME "testName1"
#define LOCAL_TEST_DESCID   45


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
dataParticle_new_test(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	dataParticle_t dataParticle;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataParticle = dataParticle_new(LOCAL_TEST_DESCNAME,
	                                LOCAL_TEST_DESCID,
	                                0);

	if (strcmp(dataParticle->name, LOCAL_TEST_DESCNAME) != 0)
		hasPassed = false;
	if (dataParticle->dataParticleID != LOCAL_TEST_DESCID)
		hasPassed = false;
	// The number of variables hint is not tested!

	dataParticle_del(&dataParticle);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataParticle_getRef_test(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	dataParticle_t dataParticle, dataParticleRef;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataParticle = dataParticle_new(LOCAL_TEST_DESCNAME,
	                                LOCAL_TEST_DESCID,
	                                0);
	dataParticleRef = dataParticle_getRef(dataParticle);

	if (strcmp(dataParticleRef->name, LOCAL_TEST_DESCNAME) != 0)
		hasPassed = false;
	if (dataParticleRef->dataParticleID != LOCAL_TEST_DESCID)
		hasPassed = false;
	// The number of variables hint is not tested!

	dataParticle_del(&dataParticle);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes == global_allocated_bytes)
		hasPassed = false;
#endif
	dataParticle_del(&dataParticleRef);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* dataParticle_getRef_test */

extern bool
dataParticle_del_test(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	dataParticle_t dataParticle;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataParticle = dataParticle_new(LOCAL_TEST_DESCNAME,
	                                LOCAL_TEST_DESCID,
	                                0);
	dataParticle_del(&dataParticle);
	if (dataParticle != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
dataParticle_addVar_test(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	dataParticle_t dataParticle;
	dataVar_t      var;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dataParticle = dataParticle_new(LOCAL_TEST_DESCNAME,
	                                LOCAL_TEST_DESCID,
	                                1);
	var = dataVar_new("Bla", DATAVARTYPE_DOUBLE, 1);

	dataParticle_addVar(dataParticle, var);
	if (varArr_getLength(dataParticle->vars) != 1)
		hasPassed = false;
	if (varArr_getElementHandle(dataParticle->vars, 0) != var)
		hasPassed = false;
	dataVar_del(&var);

	dataParticle_del(&dataParticle);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* dataParticle_addVar_test */

/*--- Implementations of local functions --------------------------------*/
