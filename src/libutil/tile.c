// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


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
extern inline void
tile_calcIdxsELAE(uint32_t numGridCells,
                  uint32_t numTiles,
                  uint32_t tileNumber,
                  uint32_t * restrict idxLo,
                  uint32_t * restrict idxHi)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);
	assert(tileNumber >= 0 && tileNumber < numTiles);
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
	*idxHi += (tileNumber < numSmallTiles ? minTileSize - 1: minTileSize);
}

extern inline void
tile_calcIdxsELAB(uint32_t numGridCells,
                  uint32_t numTiles,
                  uint32_t tileNumber,
                  uint32_t * restrict idxLo,
                  uint32_t * restrict idxHi)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);
	assert(tileNumber >= 0 && tileNumber < numTiles);
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


extern inline uint32_t
tile_calcNumLargeTilesEven(uint32_t numGridCells, uint32_t numTiles)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);

	return numGridCells % numTiles;
}

extern inline uint32_t
tile_calcNumSmallTilesEven(uint32_t numGridCells, uint32_t numTiles)
{
	return numTiles - tile_calcNumLargeTilesEven(numGridCells, numTiles);
}

extern inline uint32_t
tile_calcMinTileSizeEven(uint32_t numGridCells, uint32_t numTiles)
{
	assert(numGridCells > 0);
	assert(numTiles >= 0 && numTiles < numGridCells);

	return numGridCells / numTiles;
}

extern inline uint32_t
tile_calcMaxTileSizeEven(uint32_t numGridCells, uint32_t numTiles)
{
	if (tile_calcNumLargeTilesEven(numGridCells, numTiles) != 0)
		return tile_calcMinTileSizeEven(numGridCells, numTiles) + 1;
	else
		return tile_calcMinTileSizeEven(numGridCells, numTiles);
}
