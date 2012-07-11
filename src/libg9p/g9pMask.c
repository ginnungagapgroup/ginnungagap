// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMask.c
 * @ingroup  libg9pMask
 * @brief  Implements the mask.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMask.h"
#include <assert.h>
#include <string.h>
#include "../libutil/utilMath.h"
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pMask_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static g9pMask_t
local_allocateEmptyMask();

static void
local_setTilingFromHierarchy(g9pMask_t mask);

static void
local_allocateTilePointer(g9pMask_t mask);


/*--- Implementations of exported functions -----------------------------*/
extern g9pMask_t
g9pMask_newMinMaxTiledMask(g9pHierarchy_t hierarchy,
                           const uint8_t  maskLevel,
                           const uint8_t  minLevel,
                           const uint8_t  maxLevel,
                           const uint8_t  tileLevel)
{
	g9pMask_t mask = local_allocateEmptyMask();

	mask->hierarchy = hierarchy;
	mask->maskLevel = maskLevel;
	mask->minLevel  = minLevel;
	mask->maxLevel  = maxLevel;
	mask->tileLevel = tileLevel;

	local_setTilingFromHierarchy(mask);
	local_allocateTilePointer(mask);

	return mask;
}

extern void
g9pMask_del(g9pMask_t *mask)
{
	assert(mask != NULL && *mask != NULL);

	for (uint32_t i = 0; i < (*mask)->totalNumTiles; i++)
		if ((*mask)->maskTiles[i] != NULL)
			xfree((*mask)->maskTiles[i]);
	xfree((*mask)->maskTiles);
	g9pHierarchy_del(&((*mask)->hierarchy));

	xfree(*mask);
	*mask = NULL;
}

extern uint64_t
g9pMask_getNumCellsInMask(const g9pMask_t mask)
{
	assert(mask != NULL);

	uint64_t numCells = g9pHierarchy_getDim1DAtLevel(mask->hierarchy,
	                                                 mask->maskLevel);

	return POW_NDIM(numCells);
}

extern uint64_t
g9pMask_getNumCellsInMaskTile(const g9pMask_t mask)
{
	return g9pMask_getMaxNumCellsInTileForLevel(mask, mask->maskLevel);
}

extern uint64_t
g9pMask_getMaxNumCellsInTileForLevel(const g9pMask_t mask,
                                     uint8_t         level)
{
	assert(mask != NULL);

	level = CLIP(level, mask->minLevel, mask->maxLevel);

	uint64_t numCells;

	numCells = g9pHierarchy_getFactorBetweenLevel(mask->hierarchy,
	                                              mask->tileLevel,
	                                              level);

	return POW_NDIM(numCells);
}

extern uint64_t *
g9pMask_getMaxNumCellsInTile(const g9pMask_t mask,
                             uint64_t        *numCells)
{
	assert(mask != NULL);

	uint8_t numLevel = mask->maxLevel - mask->minLevel + 1;

	if (numCells == NULL)
		numCells = xmalloc(sizeof(uint64_t) * numLevel);

	for (uint8_t i = 0; i < numLevel; i++) {
		numCells[i] = g9pHierarchy_getFactorBetweenLevel(mask->hierarchy,
		                                                 mask->tileLevel,
		                                                 mask->minLevel + i);
		numCells[i] = POW_NDIM(numCells[i]);
	}

	return numCells;
}

extern uint64_t
g9pMask_getNumCellsInTileForLevel(const g9pMask_t mask,
                                  uint32_t        tile,
                                  uint8_t         level)
{
	assert(mask != NULL);
	assert(tile < mask->totalNumTiles);
	assert(level >= mask->minLevel && level <= mask->maxLevel);

	uint64_t numCells = UINT64_C(0);

	if (mask->maskTiles[tile] == NULL) {
		if (level == mask->minLevel)
			numCells = g9pMask_getMaxNumCellsInTileForLevel(mask, level);
		return numCells;
	}

	uint64_t     numCellsInTile = g9pMask_getNumCellsInMaskTile(mask);
	const int8_t *thisTileData  = mask->maskTiles[tile];
#ifdef WITH_OPENMP
#  pragma omp parallel for reduction(+:numCells)
#endif
	for (uint64_t i = 0; i < numCellsInTile; i++) {
		if (thisTileData[i] == level)
			numCells++;
	}

	return numCells;
}

extern uint64_t *
g9pMask_getNumCellsInTile(const g9pMask_t mask,
                          uint32_t        tile,
                          uint64_t        *numCells)
{
	assert(mask != NULL);
	assert(tile < mask->totalNumTiles);

	uint8_t numLevel = mask->maxLevel - mask->minLevel + 1;

	if (numCells == NULL)
		numCells = xmalloc(sizeof(uint64_t) * numLevel);

	for (uint8_t i = 0; i < numLevel; i++)
		numCells[i] = UINT64_C(0);

	if (mask->maskTiles[tile] == NULL) {
		numCells[0] = g9pMask_getMaxNumCellsInTileForLevel(mask,
		                                                   mask->minLevel);
		return numCells;
	}

	uint64_t     numCellsInTile = g9pMask_getNumCellsInMaskTile(mask);
	const int8_t *thisTileData  = mask->maskTiles[tile];
#ifdef WITH_OPENMP
#  pragma omp parallel for reduction(+:numCells)
#endif
	for (uint64_t i = 0; i < numCellsInTile; i++) {
		numCells[thisTileData[i] - mask->minLevel]++;
	}

	return numCells;
}

/*--- Implementations of local functions --------------------------------*/

static g9pMask_t
local_allocateEmptyMask()
{
	g9pMask_t mask = xmalloc(sizeof(struct g9pMask_struct));

	mask->totalNumTiles = 0;
	mask->maskTiles     = NULL;

	return mask;
}

static void
local_setTilingFromHierarchy(g9pMask_t mask)
{
	assert(mask->tileLevel <= mask->minLevel);

	uint32_t dimTileLevel = g9pHierarchy_getDim1DAtLevel(mask->hierarchy,
	                                                     mask->tileLevel);
	mask->totalNumTiles = 1;
	for (int i = 0; i < NDIM; i++) {
		mask->numTiles[i]    = dimTileLevel;
		mask->totalNumTiles *= dimTileLevel;
	}
}

static void
local_allocateTilePointer(g9pMask_t mask)
{
	assert(mask->totalNumTiles > 0);
	assert(mask->maskTiles == NULL);

	mask->maskTiles = xmalloc(sizeof(int8_t *) * mask->totalNumTiles);
	for (size_t i = 0; i < mask->totalNumTiles; i++) {
		mask->maskTiles[i] = NULL;
	}
	mask->isEmpty = true;
}
