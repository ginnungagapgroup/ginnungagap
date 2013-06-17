// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASK_H
#define G9PMASK_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMask.h
 * @ingroup  libg9pMask
 * @brief  Provides the interface to the mask.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "g9pHierarchy.h"
#include "../libgrid/gridPoint.h"
#include "../libgrid/gridRegular.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct g9pMask_struct *g9pMask_t;


/*--- Exported defines --------------------------------------------------*/
#define G9PMASK_NO_TILING    NULL
#define G9PMASK_IS_EMPTY     true
#define G9PMASK_IS_NOT_EMPTY false


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 * @{
 */

extern g9pMask_t
g9pMask_newMinMaxTiledMask(g9pHierarchy_t hierarchy,
                           const uint8_t  maskLevel,
                           const uint8_t  minLevel,
                           const uint8_t  maxLevel,
                           const uint8_t  tileLevel);

extern g9pMask_t
g9pMask_getRef(g9pMask_t m);

extern void
g9pMask_del(g9pMask_t *g9pMask);


/** @} */

/**
 * @name  Member Accessors
 * @{
 */

extern uint8_t
g9pMask_getMaskLevel(const g9pMask_t mask);

extern uint8_t
g9pMask_getMinLevel(const g9pMask_t mask);

extern uint8_t
g9pMask_getMaxLevel(const g9pMask_t mask);

extern uint8_t
g9pMask_getTileLevel(const g9pMask_t mask);

extern uint32_t
g9pMask_getTotalNumTiles(const g9pMask_t mask);

extern const uint32_t *
g9pMask_getNumTiles(const g9pMask_t mask);

extern int8_t *
g9pMask_getTileData(g9pMask_t mask, uint32_t tile);

extern int8_t *
g9pMask_setTileData(g9pMask_t mask, uint32_t tile, int8_t *data);

extern g9pHierarchy_t
g9pMask_getHierarchyRef(g9pMask_t mask);

extern int8_t
g9pMask_getNumLevel(const g9pMask_t mask);

/** @} */

/**
 * @name  Getter: Mask Level Specific
 * @{
 */

extern uint32_t
g9pMask_getDim1D(const g9pMask_t mask);

extern uint64_t
g9pMask_getNumCellsInMask(const g9pMask_t mask);

extern uint64_t
g9pMask_getNumCellsInMaskTile(const g9pMask_t mask);


/** @} */

/**
 * @name  Getter: Theoretical Number of Cells in Tile
 * @{
 */

extern uint64_t
g9pMask_getMaxNumCellsInTileForLevel(const g9pMask_t mask,
                                     uint8_t         level);

extern uint64_t *
g9pMask_getMaxNumCellsInTile(const g9pMask_t mask,
                             uint64_t        *numCells);


/** @} */

/**
 * @name  Getter: Actual Cell Counts
 * @{
 */

extern uint64_t
g9pMask_getNumCellsInTileForLevel(const g9pMask_t mask,
                                  uint32_t        tile,
                                  uint8_t         level);

extern uint64_t *
g9pMask_getNumCellsInTile(const g9pMask_t mask,
                          uint32_t        tile,
                          uint64_t        *numCells);

extern uint64_t *
g9pMask_getNumCellsTotal(const g9pMask_t mask, uint64_t *numCells);


/** @} */

/**
 * @name  Convenience Functions
 * @{
 */

extern gridRegular_t
g9pMask_getEmptyGridStructure(const g9pMask_t mask);

extern gridPatch_t
g9pMask_getEmptyPatchForTile(const g9pMask_t mask, const uint32_t tile);


/** @} */

/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pMask Mask
 * @ingroup libg9p
 * @brief Provides the mask.
 */

#endif
