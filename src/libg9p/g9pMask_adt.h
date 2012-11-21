// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASK_ADT_H
#define G9PMASK_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMask_adt.h
 * @ingroup  libg9pMask
 * @brief  Provides the main structure for the mask.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchy.h"
#include "../libutil/refCounter.h"
#include "../libgrid/gridPoint.h"


/*--- ADT implementation ------------------------------------------------*/
struct g9pMask_struct {
	/// @brief The reference counter.
	refCounter_t refCounter;
	g9pHierarchy_t    hierarchy;
	uint8_t           maskLevel;
	uint8_t           minLevel;
	uint8_t           maxLevel;
	uint8_t           tileLevel;

	uint32_t          totalNumTiles;
	gridPointUint32_t numTiles;
	int8_t            **maskTiles;
	bool              isEmpty;
};


#endif
