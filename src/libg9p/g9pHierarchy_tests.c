// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchy_tests.c
 * @ingroup  libg9pHierarchyTest
 * @brief  Implements the tests for @ref libg9pHierarchy.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchy_tests.h"
#include "g9pHierarchy.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pHierarchy_adt.h"


/*--- Local defines -----------------------------------------------------*/
static g9pHierarchy_t
local_getComplexHierarchy(void);


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pHierarchy_verifyComplexCreationMethodsAreIdentical(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pHierarchy_t h1, h2;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	const uint8_t  numLevels = 8;
	const uint32_t dims[]    = {32, 96, 192, 576, 1152, 5760, 17280, 34560};
	const uint32_t minDim1D  = 32;
	const uint32_t factors[] = {3, 2, 3, 2, 5, 3, 2};

	if (rank == 0)
		printf("Running %s... ", __func__);

	h1 = g9pHierarchy_newWithDims(numLevels, dims);
	h2 = g9pHierarchy_newWithExplicitFactors(numLevels, minDim1D, factors);

	const uint32_t *h1Dims = g9pHierarchy_getDim1Ds(h1, NULL);
	uint32_t       h2Dims[numLevels];
	g9pHierarchy_getDim1Ds(h2, h2Dims);
	for (int i = 0; i < numLevels; ++i) {
		if ((h1Dims[i] != h2Dims[i]) || (h1Dims[i] != dims[i]))
			hasPassed = false;
	}

	const uint32_t *h1Factors = g9pHierarchy_getFactors(h1, NULL);
	uint32_t       h2Factors[numLevels];
	g9pHierarchy_getFactors(h2, h2Factors);
	if ((h1Factors[0] != h2Factors[0]) || (h1Factors[0] != 1))
		hasPassed = false;
	for (int i = 1; i < numLevels; ++i) {
		if ((h1Factors[i] != h2Factors[i])
		    || (h1Factors[i] != factors[i - 1]))
			hasPassed = false;
	}

	g9pHierarchy_del(&h2);
	g9pHierarchy_del(&h1);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pHierarchyTests_verifyComplexCreationMethodsAreIdentical */

extern bool
g9pHierarchy_verifySimpleCreationMethodsAreIdentical(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pHierarchy_t h1, h2;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	const uint8_t  numLevels = 8;
	const uint32_t dims[]    = {32, 64, 128, 256, 512, 1024, 2048, 4096};
	const uint32_t minDim1D  = 32;
	const uint32_t factor    = 2;

	if (rank == 0)
		printf("Running %s... ", __func__);

	h1 = g9pHierarchy_newWithDims(numLevels, dims);
	h2 = g9pHierarchy_newWithSimpleFactor(numLevels, minDim1D, factor);

	const uint32_t *h1Dims = g9pHierarchy_getDim1Ds(h1, NULL);
	uint32_t       h2Dims[numLevels];
	g9pHierarchy_getDim1Ds(h2, h2Dims);
	for (int i = 0; i < numLevels; ++i) {
		if ((h1Dims[i] != h2Dims[i]) || (h1Dims[i] != dims[i]))
			hasPassed = false;
	}

	const uint32_t *h1Factors = g9pHierarchy_getFactors(h1, NULL);
	uint32_t       h2Factors[numLevels];
	g9pHierarchy_getFactors(h2, h2Factors);
	if ((h1Factors[0] != h2Factors[0]) || (h1Factors[0] != 1))
		hasPassed = false;
	for (int i = 1; i < numLevels; ++i) {
		if ((h1Factors[i] != h2Factors[i]) || (h1Factors[i] != factor))
			hasPassed = false;
	}

	g9pHierarchy_del(&h2);
	g9pHierarchy_del(&h1);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pHierarchy_verifySimpleCreationMethodsAreIdentical */

extern bool
g9pHierarchy_verifyDimsSelections(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pHierarchy_t h;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Running %s... ", __func__);

	h = local_getComplexHierarchy();

	const uint32_t *dims = g9pHierarchy_getDim1Ds(h, NULL);
	if (g9pHierarchy_getDim1DAtLevel(h, 0) != dims[0])
		hasPassed = false;
	if (g9pHierarchy_getDim1DAtLevel(h, 3) != dims[3])
		hasPassed = false;
	if (g9pHierarchy_getDim1DAtLevel(h, 6) != dims[6])
		hasPassed = false;

	g9pHierarchy_del(&h);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pHierarchy_verifyDimsSelections */

extern bool
g9pHierarchy_verifyFactorSelections(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pHierarchy_t h;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Running %s... ", __func__);

	h = local_getComplexHierarchy();
	const uint32_t *dims = g9pHierarchy_getDim1Ds(h, NULL);

	if (dims[1] / dims[0] != g9pHierarchy_getFactorToNextLevel(h, 0))
		hasPassed = false;

	if (dims[1] / dims[0] != g9pHierarchy_getFactorFromPrevLevel(h, 1))
		hasPassed = false;

	if (dims[5] / dims[1] != g9pHierarchy_getFactorBetweenLevel(h, 5, 1))
		hasPassed = false;

	if (dims[4] / dims[0] != g9pHierarchy_getFactorBetweenLevel(h, 0, 4))
		hasPassed = false;

	g9pHierarchy_del(&h);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pHierarchy_verifyFactorSelections */

extern bool
g9pHierarchy_verifyReferenceCounting(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pHierarchy_t h1, h2, h3;
#ifdef XMEM_TRACK_MEM
	size_t         allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Running %s... ", __func__);

	h1 = local_getComplexHierarchy();
	h2 = g9pHierarchy_getRef(h1);
	if (h1 != h2)
		hasPassed = false;
	g9pHierarchy_del(&h1);
	if (h1 == h2)
		hasPassed = false;

	h3 = g9pHierarchy_getRef(h2);
	const uint32_t *dimsH2 = g9pHierarchy_getDim1Ds(h2, NULL);
	const uint32_t *dimsH3 = g9pHierarchy_getDim1Ds(h3, NULL);

	if (dimsH2 != dimsH3)
		hasPassed = false;

	g9pHierarchy_del(&h3);

	if (dimsH2[2] != dimsH3[2])
		hasPassed = false;

	g9pHierarchy_del(&h2);

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
