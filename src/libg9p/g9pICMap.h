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

/**
 * @param[in]  numFiles
 *                The number of files for which to create the mapping. Must
 *                be at least @c 1 and at most equal to the number of tiles
 *                in the @c mask.
 * @param[in]  numGasLevel
 *                The number of levels containing gas particles, also the
 *                length of the @c *gasLevel array. Must be less or equal to
 *                the number of levels in the hierarchy.
 * @param[in]  *gasLevel
 *                Array of length @c numGasLevel which holds the level
 *                numbers on which gas particles are to be generated. This
 *                may be @c NULL if no gas particles are to be generated
 *                (and @c numGasLevel is @c 0).
 * @param[in]  mask
 *                The mask to use (giving the realisation of the levels).
 *                Must be a valid mask. The map object takes control of the
 *                mask and will free it when it is destroyed itself.
 */
extern g9pICMap_t
g9pICMap_new(uint32_t     numFiles,
             uint32_t     numGasLevel,
             const int8_t *gasLevel,
             g9pMask_t    mask,
             uint32_t zoomlevel);

extern void
g9pICMap_del(g9pICMap_t *g9pICMap);

extern uint32_t
g9pICMap_getFileForTile(const g9pICMap_t map, const uint32_t tile);

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
 * The IC Map encapsulates the mapping between the tiles in a mask and the
 * tiles in file.  IOW, the aim is to be able to ask two questions, firstly,
 * in which file is a given tile, and secondly, which tiles are in a given
 * file.
 */

#endif
