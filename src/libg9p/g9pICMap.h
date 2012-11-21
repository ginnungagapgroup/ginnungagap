// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PICMAP_H
#define G9PICMAP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pICMap.h
 * @ingroup  libg9pICMap
 * @brief  Provides the interface to the IC map.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMask.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct g9pICMap_struct *g9pICMap_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern g9pICMap_t
g9pICMap_new(uint32_t  numFiles,
             uint32_t  numGasLevel,
             int8_t    *gasLevel,
             g9pMask_t mask);

extern void
g9pICMap_del(g9pICMap_t *g9pICMap);

extern uint32_t
g9pICMap_getFirstTileInFile(const g9pICMap_t map, const uint32_t file);

extern uint32_t
g9pICMap_getLastTileInFile(const g9pICMap_t map, const uint32_t file);

extern const uint64_t *
g9pICMap_getNumCellsPerLevelInFile(const g9pICMap_t map,
                                   const uint32_t   file);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pICMap IC Map
 * @ingroup libg9p
 * @brief Provides the IC Map.
 *
 * The IC Map encapsulates the mapping
 */

#endif
