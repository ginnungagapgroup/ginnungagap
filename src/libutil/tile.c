// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/tile.c
 * @ingroup libutilMiscTile
 * @brief  This file provides the implementation of the tiling.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "tile.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>


/*--- Implementations of exported functios ------------------------------*/
extern void
tile_calcIdxsELAE(uint32_t           numGridCells,
                  uint32_t           numTiles,
                  uint32_t           tileNumber,
                  uint32_t *restrict idxLo,
                  uint32_t *restrict idxHi)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);
	assert(tileNumber < numTiles);
	assert(idxLo != NULL);
	assert(idxHi != NULL);

	uint32_t numSmallTiles = tile_calcNumSmallTilesEven(numGridCells,
	                                                    numTiles);
	uint32_t minTileSize   = tile_calcMinTileSizeEven(numGridCells,
	                                                  numTiles);

	*idxLo = tileNumber * minTileSize;
	if (tileNumber > numSmallTiles) // actually '>=' but '>' saves a '+ 0'
		*idxLo += tileNumber - numSmallTiles;

	*idxHi  = *idxLo;
	*idxHi += (tileNumber < numSmallTiles ? minTileSize - 1 : minTileSize);
}

extern uint32_t
tile_calcTileNumberForIdxELAE(uint32_t numGridCells,
                              uint32_t numTiles,
                              uint32_t idx)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);
	assert(idx <= numGridCells);

	uint32_t tileNumber;
	uint32_t minTileSize = tile_calcMinTileSizeEven(numGridCells,
	                                                numTiles);
	if (idx < minTileSize) {
		tileNumber = 0;
	} else {
		uint32_t numSmallTiles = tile_calcNumSmallTilesEven(numGridCells,
		                                                    numTiles);
		if (idx < numSmallTiles * minTileSize) {
			tileNumber = idx / minTileSize;
		} else {
			uint32_t maxTileSize = tile_calcMaxTileSizeEven(numGridCells,
			                                                numTiles);
			idx       -= numSmallTiles * minTileSize;
			tileNumber = idx / (maxTileSize) + numSmallTiles;
		}
	}

	return tileNumber;
}

extern void
tile_calcIdxsELAB(uint32_t           numGridCells,
                  uint32_t           numTiles,
                  uint32_t           tileNumber,
                  uint32_t *restrict idxLo,
                  uint32_t *restrict idxHi)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);
	assert(tileNumber < numTiles);
	assert(idxLo != NULL);
	assert(idxHi != NULL);

	uint32_t numLargeTiles = tile_calcNumLargeTilesEven(numGridCells,
	                                                    numTiles);
	uint32_t minTileSize   = tile_calcMinTileSizeEven(numGridCells,
	                                                  numTiles);

	*idxLo = tileNumber * minTileSize;
	if (tileNumber < numLargeTiles)
		*idxLo += tileNumber;
	else
		*idxLo += numLargeTiles;

	*idxHi  = *idxLo;
	*idxHi += (tileNumber < numLargeTiles ? minTileSize : minTileSize - 1);
}

extern uint32_t
tile_calcTileNumberForIdxELAB(uint32_t numGridCells,
                              uint32_t numTiles,
                              uint32_t idx)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);
	assert(idx <= numGridCells);

	uint32_t tileNumber;
	uint32_t maxTileSize = tile_calcMaxTileSizeEven(numGridCells,
	                                                numTiles);
	if (idx < maxTileSize) {
		tileNumber = 0;
	} else {
		uint32_t numLargeTiles = tile_calcNumLargeTilesEven(numGridCells,
		                                                    numTiles);
		if (idx < numLargeTiles * maxTileSize) {
			tileNumber = idx / maxTileSize;
		} else {
			uint32_t minTileSize = tile_calcMinTileSizeEven(numGridCells,
			                                                numTiles);
			idx       -= numLargeTiles * maxTileSize;
			tileNumber = idx / (minTileSize) + numLargeTiles;
		}
	}

	return tileNumber;
}
