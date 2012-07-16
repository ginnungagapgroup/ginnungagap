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
#include "../libutil/lIdx.h"
#include "../libutil/tile.h"


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


/*--- Implementations: Creating and Deleting ----------------------------*/
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

/*--- Implementations: Member Accessors ---------------------------------*/
extern uint8_t
g9pMask_getMaskLevel(const g9pMask_t mask)
{
	assert(mask != NULL);

	return mask->maskLevel;
}

extern uint8_t
g9pMask_getMinLevel(const g9pMask_t mask)
{
	assert(mask != NULL);

	return mask->minLevel;
}

extern uint8_t
g9pMask_getMaxLevel(const g9pMask_t mask)
{
	assert(mask != NULL);

	return mask->maxLevel;
}

extern uint8_t
g9pMask_getTileLevel(const g9pMask_t mask)
{
	assert(mask != NULL);

	return mask->tileLevel;
}

extern uint32_t
g9pMask_getTotalNumTiles(const g9pMask_t mask)
{
	assert(mask != NULL);

	return mask->totalNumTiles;
}

extern const uint32_t *
g9pMask_getNumTiles(const g9pMask_t mask)
{
	assert(mask != NULL);

	return mask->numTiles;
}

extern int8_t *
g9pMask_getTileData(g9pMask_t mask, uint32_t tile)
{
	assert(mask != NULL);
	assert(tile < mask->totalNumTiles);

	return mask->maskTiles[tile];
}

extern int8_t *
g9pMask_setTileData(g9pMask_t mask, uint32_t tile, int8_t *data)
{
	assert(mask != NULL);
	assert(tile < mask->totalNumTiles);

	int8_t *oldData = NULL;

	if (mask->maskTiles[tile] != data) {
		oldData               = mask->maskTiles[tile];
		mask->maskTiles[tile] = data;
	}

	return oldData;
}

extern g9pHierarchy_t
g9pMask_getHierarchyRef(g9pMask_t mask)
{
	assert(mask != NULL);

	return g9pHierarchy_getRef(mask->hierarchy);
}

/*--- Implementations: Getter, Mask Level Specific ----------------------*/
extern uint32_t
g9pMask_getDim1D(const g9pMask_t mask)
{
	assert(mask != NULL);

	return g9pHierarchy_getDim1DAtLevel(mask->hierarchy, mask->maskLevel);
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

/*--- Implementations: Getter, Theoretical Number of Cells in Tile ------*/
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

/*--- Implementations: Getter, Actual Cell Counts -----------------------*/
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

	const int8_t *thisTileData  = mask->maskTiles[tile];

	const uint64_t nCTL  = g9pMask_getMaxNumCellsInTileForLevel(mask, level);
	const uint64_t nCTM  = g9pMask_getNumCellsInMaskTile(mask);
	const uint64_t nCFac = nCTL < nCTM ? nCTM / nCTL : nCTL / nCTM;

	for (uint64_t i = 0; i < nCTL; i++) {
		if (thisTileData[i] == level)
			numCells++;
	}

	if (nCTL < nCTM) {
		assert(numCells % nCFac == 0);
		numCells /= nCFac;
	} else {
		numCells *= nCFac;
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

	for (uint8_t i = mask->minLevel; i < mask->maskLevel; i++) {
		uint64_t factor = g9pHierarchy_getFactorBetweenLevel(mask->hierarchy,
		                                                     i,
		                                                     mask->maskLevel);
		factor = POW_NDIM(factor);
		assert(numCells[i - mask->minLevel] % factor == 0);
		numCells[i - mask->minLevel] /= factor;
	}
	for (uint8_t i = mask->maskLevel + 1; i <= mask->maxLevel; i++) {
		uint64_t factor = g9pHierarchy_getFactorBetweenLevel(mask->hierarchy,
		                                                     i,
		                                                     mask->maskLevel);
		factor = POW_NDIM(factor);
		numCells[i - mask->minLevel] *= factor;
	}

	return numCells;
}

/*--- Implementations: Convenience Functions ----------------------------*/
extern gridRegular_t
g9pMask_getEmptyGridStructure(const g9pMask_t mask)
{
	gridPointDbl_t    origin, extent;
	gridPointUint32_t dims;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = 1.0;
		dims[i]   = g9pMask_getDim1D(mask);
	}

	gridRegular_t grid = gridRegular_new("Mask", origin, extent, dims);
	gridRegular_attachVar(grid, dataVar_new("Mask", DATAVARTYPE_INT8, 1));

	for (uint32_t i = 0; i < mask->totalNumTiles; i++) {
		gridPointUint32_t idxLo, idxHi, tilePos;
		lIdx_toCoord3d(i, mask->numTiles, tilePos);
		tile_calcNDIdxsELAE(NDIM, dims, mask->numTiles, tilePos,
		                    idxLo, idxHi);
		gridRegular_attachPatch(grid, gridPatch_new(idxLo, idxHi));
	}

	return grid;
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
