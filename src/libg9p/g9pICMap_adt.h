// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PICMAP_ADT_H
#define G9PICMAP_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file ilibg9p/g9pICMap_adt.h
 * @ingroup  libg9pICMap
 * @brief  Provides the main structure of the IC map.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchy.h"


/*--- ADT implementation ------------------------------------------------*/
struct g9pICMap_struct {
	// Constructor information
	uint32_t       numFiles;
	uint32_t       numGasLevel;
	int8_t         *gasLevel;
	g9pMask_t      mask;
	g9pHierarchy_t hierarchy;
	// Computed information
	uint32_t       *firstTileIdx; // Stores for each file the first tile idx
	uint32_t       *lastTileIdx;  // Stores for each file the last tile idx
	uint64_t 		*numCells; // Stores for each file cell counts
};


#endif
