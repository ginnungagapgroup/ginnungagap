// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef TILE_H
#define TILE_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/tile.h
 * @ingroup libutilMisc
 * @brief  This file provides the interface for tiling.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include <assert.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name ELAE Method
 *
 * The ELAE (even, large at end) methods ensures that the tile sizes
 * differ by at most one cell and the larger tiles are at the end.  This
 * means that for a grid with 27 cells that is split in 5 tiles,
 * the first 3 tiles have 5 cells each and the last 2 tiles have 6 cells
 * each.
 *
 * @{
 */

/**
 * @brief  Calculates the indices for a given tile.
 *
 * @param[in]   numGridCells
 *                 The number of cells.  This must be a positive value.
 * @param[in]   numTiles
 *                 The number of tiles the @c nCells cells are spread
 *                 over.  This must be a positive number less or equal
 *                 to @c numGridCells.
 * @param[in]   tileNumber
 *                 The number of the tile to find the minimun and
 *                 maximum indices for.  This must be a value between
 *                 (inclusive) @c 0 and (exclusive) @c numTiles.
 * @param[out]  *idxLo
 *                 Pointer to the external storage position that will
 *                 receive the minimum index.  Passing @c NULL is
 *                 undefined.
 * @param[out]  *idxHi
 *                 Like @c IdxLo but for the maximum index.
 *
 * @return  Returns nothing.
 */
extern void
tile_calcIdxsELAE(uint32_t           numGridCells,
                  uint32_t           numTiles,
                  uint32_t           tileNumber,
                  uint32_t *restrict idxLo,
                  uint32_t *restrict idxHi);

/**
 * @brief  Calculates the tile a given index is located in.
 *
 * @param[in]  numGridCells
 *                The number of cells.
 * @param[in]  numTiles
 *                The number of tiles.
 * @param[in]  idx
 *                The index for which to locate the tile.
 *
 * @return  Returns the corresponding tile number.
 */
extern uint32_t
tile_calcTileNumberForIdxELAE(uint32_t numGridCells,
                              uint32_t numTiles,
                              uint32_t idx);


/** @} */

/**
 * @name ELAB Method
 *
 * The ELAB (even, large at beginning) methods ensures that the tile sizes
 * differ by at most one cell and the larger tiles are at the beginning.
 * This  means that for a grid with 27 cells that is split in 5 tiles,
 * the first 2 tiles have 6 cells each and the last 3 tiles have 5 cells
 * each.
 *
 * @{
 */

/** \copydoc tile_calcIdxsELAE() */
extern void
tile_calcIdxsELAB(uint32_t           numGridCells,
                  uint32_t           numTiles,
                  uint32_t           tileNumber,
                  uint32_t *restrict idxLo,
                  uint32_t *restrict idxHi);

/** \copydoc tile_calcTileNumberForIdxELAE() */
extern uint32_t
tile_calcTileNumberForIdxELAB(uint32_t numGridCells,
                              uint32_t numTiles,
                              uint32_t idx);


/** @} */

/**
 * @name  Common Functions for ELAE and ELAB
 *
 * @{
 */

/**
 * @brief  Calculates the number of cells in the large tiles for evenly
 *         tiled methods.
 *
 * @param[in]  numGridCells
 *                 The number of cells.  This must be a positive value.
 * @param[in]  numTiles
 *                 The number of tiles the @c nCells cells are spread
 *                 over.  This must be a positive number less or equal
 *                 to @c numGridCells.
 *
 * @return  Returns the number of cells in a large tile.
 */
static inline uint32_t
tile_calcNumLargeTilesEven(uint32_t numGridCells, uint32_t numTiles)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles <= numGridCells);

	return numGridCells % numTiles;
}

/**
 * @brief  Calculates the number of cells in the small tiles for evenly
 *         tiled methods.
 *
 * @param[in]  numGridCells
 *                 The number of cells.  This must be a positive value.
 * @param[in]  numTiles
 *                 The number of tiles the @c nCells cells are spread
 *                 over.  This must be a positive number less or equal
 *                 to @c numGridCells.
 *
 * @return  Returns the number of cells in a small tile.
 */
static inline uint32_t
tile_calcNumSmallTilesEven(uint32_t numGridCells, uint32_t numTiles)
{
	return numTiles - tile_calcNumLargeTilesEven(numGridCells, numTiles);
}

/**
 * @brief  Calculates the size of the small tiles for evenly tiled
 *         methods.
 *
 * @param[in]  numGridCells
 *                 The number of cells.  This must be a positive value.
 * @param[in]   numTiles
 *                 The number of tiles the @c nCells cells are spread
 *                 over.  This must be a positive number less or equal
 *                 to @c numGridCells.
 *
 * @return  Returns the number of cells in the smallest tile.
 */
static inline uint32_t
tile_calcMinTileSizeEven(uint32_t numGridCells, uint32_t numTiles)
{
	assert(numGridCells > 0);
	assert(numTiles > 0 && numTiles < numGridCells);

	return numGridCells / numTiles;
}

/**
 * @brief  Calculates the size of the small tiles for evenly tiled
 *         methods.
 *
 * @param[in]  numGridCells
 *                 The number of cells.  This must be a positive value.
 * @param[in]   numTiles
 *                 The number of tiles the @c nCells cells are spread
 *                 over.  This must be a positive number less or equal
 *                 to @c numGridCells.
 *
 * @return  Returns the number of cells in the smallest tile.
 */
static inline uint32_t
tile_calcMaxTileSizeEven(uint32_t numGridCells, uint32_t numTiles)
{
	if (tile_calcNumLargeTilesEven(numGridCells, numTiles) != 0)
		return tile_calcMinTileSizeEven(numGridCells, numTiles) + 1;
	else
		return tile_calcMinTileSizeEven(numGridCells, numTiles);
}

/** @} */

#endif
