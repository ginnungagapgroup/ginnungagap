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


/*--- ADT handle --------------------------------------------------------*/
typedef struct g9pMask_struct *g9pMask_t;


/*--- Exported defines --------------------------------------------------*/
#define G9PMASK_NO_TILING    NULL
#define G9PMASK_IS_EMPTY     true
#define G9PMASK_IS_NOT_EMPTY false


/*--- Prototypes of exported functions ----------------------------------*/
extern g9pMask_t
g9pMask_newMinMaxTiledMask(g9pHierarchy_t hierarchy,
                           const uint8_t  maskLevel,
                           const uint8_t  minLevel,
                           const uint8_t  maxLevel,
                           const uint8_t  tileLevel);

extern void
g9pMask_del(g9pMask_t *g9pMask);

extern uint64_t
g9pMask_getNumCellsInMaskTile(const g9pMask_t mask);

extern uint64_t
g9pMask_getMaxNumCellsInTileForLevel(const g9pMask_t mask,
                                     uint8_t         level);

extern uint64_t *
g9pMask_getMaxNumCellsInTile(const g9pMask_t mask,
                             uint64_t        *numCells);

extern uint64_t
g9pMask_getNumCellsInTileForLevel(const g9pMask_t mask,
                                  uint32_t        tile,
                                  uint8_t         level);

extern uint64_t *
g9pMask_getNumCellsInTile(const g9pMask_t mask,
                          uint32_t        tile,
                          uint64_t        *numCells);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pMask Mask
 * @ingroup libg9p
 * @brief Provides the mask.
 */

#endif
