// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pIDGenerator_tests.c
 * @ingroup  libg9pIDGenerator
 * @brief  Implements the test functions for the ID generator.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pIDGenerator_tests.h"
#include "g9pIDGenerator.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "g9pIDGenerator_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static g9pHierarchy_t
local_getComplexHierarchy(void);


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pIDGenerator_verifyCreation(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	g9pIDGenerator_t g9pIDGenerator;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	g9pHierarchy_t h = g9pHierarchy_newWithSimpleFactor(7, 2, 2);

	g9pIDGenerator = g9pIDGenerator_new(h, 6);
	g9pIDGenerator_del(&g9pIDGenerator);
	if (g9pIDGenerator != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
g9pIDGenerator_verifyIDGeneration(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	g9pIDGenerator_t idGen;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	g9pHierarchy_t    h = local_getComplexHierarchy();
	g9pID_t           id;
	gridPointUint32_t coord;
	for (int i = 0; i < NDIM; i++)
		coord[i] = 1;

	idGen = g9pIDGenerator_new(h, 7);
	id    = g9pIDGenerator_calcID(idGen, coord, 0);
	if (id != UINT64_C(1289982413880))
		hasPassed = false;
	id = g9pIDGenerator_calcID(idGen, coord, 1);
	if (id != UINT64_C(429994137960))
		hasPassed = false;
	id = g9pIDGenerator_calcID(idGen, coord, 2);
	if (id != UINT64_C(214997068980))
		hasPassed = false;
	id = g9pIDGenerator_calcID(idGen, coord, 3);
	if (id != UINT64_C(71665689660))
		hasPassed = false;
	id = g9pIDGenerator_calcID(idGen, coord, 4);
	if (id != UINT64_C(35832844830))
		hasPassed = false;
	id = g9pIDGenerator_calcID(idGen, coord, 5);
	if (id != UINT64_C(7166568966))
		hasPassed = false;
	id = g9pIDGenerator_calcID(idGen, coord, 6);
	if (id != UINT64_C(2388856322))
		hasPassed = false;
	id = g9pIDGenerator_calcID(idGen, coord, 7);
	if (id != UINT64_C(1194428161))
		hasPassed = false;

	g9pIDGenerator_del(&idGen);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pIDGenerator_verifyIDGeneration */

extern bool
g9pIDGenerator_verifyIDResolving(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	g9pIDGenerator_t idGen;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	g9pHierarchy_t    h = local_getComplexHierarchy();
	gridPointUint32_t coord;

	idGen = g9pIDGenerator_new(h, 7);

	g9pIDGenerator_calcCoord(idGen, UINT64_C(1289982413880), coord, 0);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(429994137960), coord, 1);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(214997068980), coord, 2);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(71665689660), coord, 3);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(35832844830), coord, 4);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(7166568966), coord, 5);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(2388856322), coord, 6);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(1194428161), coord, 7);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(0), coord, 7);
	if ((coord[0] != 0) || (coord[1] != 0) || ((NDIM > 2) && (coord[2] != 0)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(41278242816000), coord, 7);
	if ((coord[0] != 0) || (coord[1] != 0) || ((NDIM > 2) && (coord[2] != 0)))
		hasPassed = false;

	g9pIDGenerator_calcCoord(idGen, UINT64_C(41279437244161), coord, 7);
	if ((coord[0] != 1) || (coord[1] != 1) || ((NDIM > 2) && (coord[2] != 1)))
		hasPassed = false;

	g9pIDGenerator_del(&idGen);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
static g9pHierarchy_t
local_getComplexHierarchy(void)
{
	const uint8_t  numLevels = 8;
	const uint32_t dims[]    = {32, 96, 192, 576, 1152, 5760, 17280, 34560};

	return g9pHierarchy_newWithDims(numLevels, dims);
}
