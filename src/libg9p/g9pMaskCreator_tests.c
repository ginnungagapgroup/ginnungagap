// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMaskCreator_tests.c
 * @ingroup  libg9pMaskCreatorTest
 * @brief  Implements the tests for @ref libg9pMaskCreator.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMaskCreator_tests.h"
#include "g9pMaskCreator.h"
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


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pMaskCreator_verifyEmptyMaskHasMinLevelThroughout(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	// 4 8 16 32 64 128
	g9pHierarchy_t h = g9pHierarchy_newWithSimpleFactor(6, 4, 2);
	// Mask at 32^3, minLevel at 16^3, maxLevel at 128^3, tiling at 4^3
	g9pMask_t      m = g9pMask_newMinMaxTiledMask(h, 3, 2, 5, 0);


	uint64_t count[4];

	(void)g9pMask_getNumCellsInTile(m, 0, count);
	if ((count[3] != 0) || (count[2] != 0) || (count[1] != 0))
		hasPassed = false;
	if (count[0] != POW_NDIM(4))
		hasPassed = false;

	g9pMaskCreator_fromCells(m, 0, NULL);

	(void)g9pMask_getNumCellsInTile(m, 0, count);
	if ((count[3] != 0) || (count[2] != 0) || (count[1] != 0))
		hasPassed = false;
	if (count[0] != POW_NDIM(4))
		hasPassed = false;

	g9pMask_del(&m);
	if (rank == 0)
		printf("Testing %s... ", __func__);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskCreator_verifyEmptyMaskHasMinLevelThroughout */

extern bool
g9pMaskCreator_verifyMaskIfOneCellIsTagged(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	// 2 4 8 16 32 64 128
	g9pHierarchy_t          h = g9pHierarchy_newWithSimpleFactor(7, 2, 2);
	// Mask at 32^3, minLevel at 16^3, maxLevel at 128^3, tiling at 2^3
	g9pMask_t               m = g9pMask_newMinMaxTiledMask(h, 4, 3, 6, 0);

	uint64_t                count[4];
#if (NDIM == 3)
	const gridPointUint32_t cell[2] = {{4, 4, 4}, {5, 5, 5}};
#else
	const gridPointUint32_t cell[2] = {{4, 4}, {5, 5}};
#endif

	g9pMaskCreator_fromCells(m, 1, cell);

	(void)g9pMask_getNumCellsInTile(m, 1, count);
	if (count[3] != 0)
		hasPassed = false;
	if (count[2] != 0)
		hasPassed = false;
	if (count[1] != 0)
		hasPassed = false;
	if (count[0] != POW_NDIM(8))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 0, count);
	if (count[3] != POW_NDIM(4))
		hasPassed = false;
	if (count[2] != (POW_NDIM(3) - 1) * POW_NDIM(2))
		hasPassed = false;
	if (count[1] != 189)
		hasPassed = false;
	if (count[0] != 485)
		hasPassed = false;

	g9pMask_del(&m);
	if (rank == 0)
		printf("Testing %s... ", __func__);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskCreator_verifyCorrectMaskIfOneCellIsTagged */

extern bool
g9pMaskCreator_verifyMaskIfOneCellsAffectsMultipleTiles(void)
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

#if (NDIM == 3)
	// Test only works for NDIM==3

	// level: 0   1   2   3   4   5   6
	// resol: 2   4   8  16  32  64 128
	g9pHierarchy_t          h = g9pHierarchy_newWithSimpleFactor(7, 2, 2);
	// Mask at 32^3, minLevel at 16^3, maxLevel at 128^3, tiling at 2^3
	g9pMask_t               m = g9pMask_newMinMaxTiledMask(h, 4, 3, 6, 0);

	uint64_t                count[4], count2[4];
	const gridPointUint32_t cell[1] = {{15, 15, 15}};

	g9pMaskCreator_fromCells(m, 1, cell);

	(void)g9pMask_getNumCellsInTile(m, 0, count);
	if (count[3] != POW_NDIM(4) * 1)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 7)
		hasPassed = false;
	if (count[1] != 1 * 56)
		hasPassed = false;
	if (count[0] != 4032 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 1, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 4)
		hasPassed = false;
	if (count[1] != 1 * 28)
		hasPassed = false;
	if (count[0] != 4064 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 2, count2);
	if (count[3] != count2[3])
		hasPassed = false;
	if (count[2] != count2[2])
		hasPassed = false;
	if (count[1] != count2[1])
		hasPassed = false;
	if (count[0] != count2[0])
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 3, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 2)
		hasPassed = false;
	if (count[1] != 1 * 14)
		hasPassed = false;
	if (count[0] != 4080 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 4, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 4)
		hasPassed = false;
	if (count[1] != 1 * 28)
		hasPassed = false;
	if (count[0] != 4064 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 5, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 2)
		hasPassed = false;
	if (count[1] != 1 * 14)
		hasPassed = false;
	if (count[0] != 4080 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 6, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 2)
		hasPassed = false;
	if (count[1] != 1 * 14)
		hasPassed = false;
	if (count[0] != 4080 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 7, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 1)
		hasPassed = false;
	if (count[1] != 1 * 7)
		hasPassed = false;
	if (count[0] != 4088 / POW_NDIM(2))
		hasPassed = false;

	g9pMask_del(&m);
#endif //NDIM==3


#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskCreator_verifyMaskIfOneCellsAffectsMultipleTiles */

extern bool
g9pMaskCreator_verifyMaskIfOneCellsAffectsMultTilesPeriodic(void)
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

#if (NDIM == 3)
	// Test only works for NDIM==3

	// level: 0   1   2   3   4   5   6
	// resol: 2   4   8  16  32  64 128
	g9pHierarchy_t          h = g9pHierarchy_newWithSimpleFactor(7, 2, 2);
	// Mask at 32^3, minLevel at 16^3, maxLevel at 128^3, tiling at 2^3
	g9pMask_t               m = g9pMask_newMinMaxTiledMask(h, 4, 3, 6, 0);

	uint64_t                count[4], count2[4];
	const gridPointUint32_t cell[1] = {{31, 31, 31}};

	g9pMaskCreator_fromCells(m, 1, cell);

	(void)g9pMask_getNumCellsInTile(m, 7, count);
	if (count[3] != POW_NDIM(4) * 1)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 7)
		hasPassed = false;
	if (count[1] != 1 * 56)
		hasPassed = false;
	if (count[0] != 4032 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 6, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 4)
		hasPassed = false;
	if (count[1] != 1 * 28)
		hasPassed = false;
	if (count[0] != 4064 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 5, count2);
	if (count[3] != count2[3])
		hasPassed = false;
	if (count[2] != count2[2])
		hasPassed = false;
	if (count[1] != count2[1])
		hasPassed = false;
	if (count[0] != count2[0])
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 4, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 2)
		hasPassed = false;
	if (count[1] != 1 * 14)
		hasPassed = false;
	if (count[0] != 4080 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 3, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 4)
		hasPassed = false;
	if (count[1] != 1 * 28)
		hasPassed = false;
	if (count[0] != 4064 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 2, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 2)
		hasPassed = false;
	if (count[1] != 1 * 14)
		hasPassed = false;
	if (count[0] != 4080 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 1, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 2)
		hasPassed = false;
	if (count[1] != 1 * 14)
		hasPassed = false;
	if (count[0] != 4080 / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 0, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 1)
		hasPassed = false;
	if (count[1] != 1 * 7)
		hasPassed = false;
	if (count[0] != 4088 / POW_NDIM(2))
		hasPassed = false;

	g9pMask_del(&m);
#endif //NDIM==3


#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskCreator_verifyMaskIfOneCellsAffectsMultTilesPeriodic */

extern bool
g9pMaskCreator_verifyMaskIfThreeCellsAreTaggedInOneTile(void)
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

#if (NDIM == 3)
	// Test only works for NDIM==3

	// level: 0   1   2   3   4   5   6
	// resol: 2   4   8  16  32  64 128
	g9pHierarchy_t          h = g9pHierarchy_newWithSimpleFactor(7, 2, 2);
	// Mask at 32^3, minLevel at 16^3, maxLevel at 128^3, tiling at 2^3
	g9pMask_t               m = g9pMask_newMinMaxTiledMask(h, 4, 3, 6, 0);

	uint64_t                count[4];
	const gridPointUint32_t cell[3] = {{6, 6, 6}, {6, 6, 7}, {6, 5, 6}};

	g9pMaskCreator_fromCells(m, 3, cell);

	(void)g9pMask_getNumCellsInTile(m, 1, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 0)
		hasPassed = false;
	if (count[1] != 1 * 0)
		hasPassed = false;
	if (count[0] != POW_NDIM(16) / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 5, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 0)
		hasPassed = false;
	if (count[1] != 1 * 0)
		hasPassed = false;
	if (count[0] != POW_NDIM(16) / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 0, count);
	if (count[3] != POW_NDIM(4) * 3)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 42)
		hasPassed = false;
	if (count[1] != 1 * 243)
		hasPassed = false;
	if (count[0] != (POW_NDIM(16) - (3 + 42 + 243)) / POW_NDIM(2))
		hasPassed = false;

	g9pMask_del(&m);
#endif //NDIM==3

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskCreator_verifyMaskIfThreeCellsAreTaggedInOneTile */

extern bool
g9pMaskCreator_verifyMaskIfTwoCellsAreTaggedSlightOverlap(void)
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

#if (NDIM == 3)
	// Test only works for NDIM==3

	// level: 0   1   2   3   4   5   6
	// resol: 2   4   8  16  32  64 128
	g9pHierarchy_t          h = g9pHierarchy_newWithSimpleFactor(7, 2, 2);
	// Mask at 32^3, minLevel at 16^3, maxLevel at 128^3, tiling at 2^3
	g9pMask_t               m = g9pMask_newMinMaxTiledMask(h, 4, 3, 6, 0);

	uint64_t                count[4];
	const gridPointUint32_t cell[2] = {{5, 5, 5}, {8, 8, 8}};

	g9pMaskCreator_fromCells(m, 2, cell);

	(void)g9pMask_getNumCellsInTile(m, 1, count);
	if (count[3] != POW_NDIM(4) * 0)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 0)
		hasPassed = false;
	if (count[1] != 1 * 0)
		hasPassed = false;
	if (count[0] != POW_NDIM(16) / POW_NDIM(2))
		hasPassed = false;

	(void)g9pMask_getNumCellsInTile(m, 0, count);
	if (count[3] != POW_NDIM(4) * 2)
		hasPassed = false;
	if (count[2] != POW_NDIM(2) * 52)
		hasPassed = false;
	if (count[1] != 1 * 370)
		hasPassed = false;
	if (count[0] != (POW_NDIM(16) - (2 + 52 + 370)) / POW_NDIM(2))
		hasPassed = false;

	g9pMask_del(&m);
#endif //NDIM==3

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
