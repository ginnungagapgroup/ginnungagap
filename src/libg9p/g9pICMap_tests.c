// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pICMap_tests.c
 * @ingroup  GROUP
 * @brief  SHORT DESC
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pICMap_tests.h"
#include "g9pICMap.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "g9pICMap_adt.h"
#include "g9pMaskCreator.h"
#include "g9pMask.h"
#include "g9pHierarchy.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pICMap_verifySimpleMapCreation(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	g9pICMap_t map;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	// 2 4 8 16 32 64 128
	g9pHierarchy_t h = g9pHierarchy_newWithSimpleFactor(7, 2, 2);
	// Mask at 32^3, minLevel at 16^3, maxLevel at 128^3, tiling at 2^3
	g9pMask_t      m = g9pMask_newMinMaxTiledMask(h, 4, 3, 6, 0);

	map = g9pICMap_new(3, 0, NULL, m);

	const uint64_t *numCells;
	uint32_t       firstTile, lastTile;

	for (uint32_t i = 0; i < 3; i++) {
		firstTile = g9pICMap_getFirstTileInFile(map, i);
		lastTile  = g9pICMap_getLastTileInFile(map, i);
		numCells  = g9pICMap_getNumCellsPerLevelInFile(map, i);
		if (numCells[0] != 512 * (lastTile - firstTile + 1))
			hasPassed = false;
		if (numCells[1] != 0)
			hasPassed = false;
		if (numCells[2] != 0)
			hasPassed = false;
		if (numCells[3] != 0)
			hasPassed = false;
		if (g9pICMap_getFileForTile(map, firstTile) != i)
			hasPassed = false;
		if (g9pICMap_getFileForTile(map, lastTile) != i)
			hasPassed = false;
	}

	g9pICMap_del(&map);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pICMap_verifySimpleMapCreation */

/*--- Implementations of local functions --------------------------------*/
