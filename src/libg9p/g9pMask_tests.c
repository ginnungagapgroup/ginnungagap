// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMask_tests.c
 * @ingroup  libg9pMaskTest
 * @brief  Implements the test for @ref libg9pMask.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMask_tests.h"
#include "g9pMask.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Local constants ---------------------------------------------------*/
const static uint8_t  g_numLevels = 12;
const static uint32_t g_dims[]
    = {2, 4, 8, 32, 96, 192, 576, 1152, 5760, 17280, 34560, 69120};


/*--- Prototypes of local functions -------------------------------------*/
static g9pHierarchy_t
local_getHierarchy(void);


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pMask_verifyCreationOfMinMaxMask(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	g9pMask_t mask;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	g9pHierarchy_t h = local_getHierarchy();
	mask = g9pMask_newMinMaxTiledMask(h, 5, 3, 9, 2);

	uint64_t expected = POW_NDIM(g_dims[5]);
	if (g9pMask_getNumCellsInMask(mask) != expected)
		hasPassed = false;

	uint64_t tileCells = POW_NDIM(g_dims[5]) / POW_NDIM(g_dims[2]);
	if (g9pMask_getNumCellsInMaskTile(mask) != tileCells)
		hasPassed = false;

	if (g9pMask_getDim1D(mask) != g_dims[5])
		hasPassed = false;

	g9pMask_del(&mask);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMask_verifyCreationOfMinMaxMask */

extern bool
g9pMask_verifyMaxNumCells(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	g9pMask_t mask;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	g9pHierarchy_t h = local_getHierarchy();
	mask = g9pMask_newMinMaxTiledMask(h, 5, 3, 9, 2);

	// Tests clipping
	if (g9pMask_getMaxNumCellsInTileForLevel(mask, 2)
	    != g9pMask_getMaxNumCellsInTileForLevel(mask, 3))
		hasPassed = false;

	uint64_t tmp[g_numLevels];
	g9pMask_getMaxNumCellsInTile(mask, tmp);

	if (tmp[4] != POW_NDIM(g_dims[7]) / POW_NDIM(g_dims[2]))
		hasPassed = false;

	g9pMask_del(&mask);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMask_verifyMaxNumCells */

extern bool
g9pMask_verifyNumCellsEmptyMask(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	g9pMask_t mask;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	g9pHierarchy_t h = local_getHierarchy();
	mask = g9pMask_newMinMaxTiledMask(h, 5, 3, 9, 2);

	// Empty tile means populate completely with minLevel
	if (g9pMask_getNumCellsInTileForLevel(mask, 2, 3) == UINT64_C(0))
		hasPassed = false;
	uint64_t numCellsMinLevel = POW_NDIM(g_dims[3]) / POW_NDIM(g_dims[2]);
	if (g9pMask_getNumCellsInTileForLevel(mask, 2, 3) != numCellsMinLevel)
		hasPassed = false;

	// Empty tile shouldn't have cells on level > minLevel
	if (g9pMask_getNumCellsInTileForLevel(mask, 2, 8) != UINT64_C(0))
		hasPassed = false;

	// Now test getting all numCells, only minLevel should have cells
	uint64_t expected[7] = {numCellsMinLevel, UINT64_C(0), UINT64_C(0),
		                    UINT64_C(0),      UINT64_C(0), UINT64_C(0),
		                    UINT64_C(0)};
	uint64_t *tmp = g9pMask_getNumCellsInTile(mask, 2, NULL);

	if (tmp[0] != expected[0])
		hasPassed = false;

	if (tmp[6] != expected[6])
		hasPassed = false;

	g9pMask_getMaxNumCellsInTile(mask, tmp);
	if (tmp[4] != POW_NDIM(g_dims[7]) / POW_NDIM(g_dims[2]))
		hasPassed = false;

	xfree(tmp);
	g9pMask_del(&mask);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMask_verifyNumCellsEmptyMask */

extern bool
g9pMask_verifyCreationOfGridStructure(void)
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

	g9pHierarchy_t h    = local_getHierarchy();
	g9pMask_t      mask = g9pMask_newMinMaxTiledMask(h, 5, 3, 9, 2);
	gridRegular_t  grid = g9pMask_getEmptyGridStructure(mask);

	uint32_t numPatches = gridRegular_getNumPatches(grid);
	if (g9pMask_getTotalNumTiles(mask) != gridRegular_getNumPatches(grid))
		hasPassed = false;

	uint64_t numCellsInTile = g9pMask_getNumCellsInMaskTile(mask);
	
	gridPatch_t patch;
	for (uint32_t i = 0 ; i<numPatches; i++) {
		patch = gridRegular_getPatchHandle(grid, i);
		if (gridPatch_getNumCells(patch) != numCellsInTile)
			hasPassed = false;
	}

	gridPointUint32_t idxLo;
	gridPatch_getIdxLo(patch, idxLo);
	for (int i = 0; i<NDIM; i++) {
		if (idxLo[i] != 168) // 24 cells per tile, 192 cell total (1D)
			hasPassed = false;
	}

	gridRegular_del(&grid);
	g9pMask_del(&mask);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
g9pMask_verifyDelete(void)
{
	bool hasPassed  = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	g9pHierarchy_t h    = local_getHierarchy();
	g9pMask_t      mask = g9pMask_newMinMaxTiledMask(h, 5, 3, 9, 2);
	g9pMask_t      mask2 = g9pMask_getRef(mask);

	if (mask != mask2)
		hasPassed = false;

	g9pMask_del(&mask);
	if (mask != NULL || mask == mask2)
		hasPassed = false;

	mask = g9pMask_getRef(mask2);
	if (mask != mask2)
		hasPassed = false;

	g9pMask_del(&mask);
	g9pMask_del(&mask2);

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
	return g9pHierarchy_newWithDims(g_numLevels, g_dims);
}
