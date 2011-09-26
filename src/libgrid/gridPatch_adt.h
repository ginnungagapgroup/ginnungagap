// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPATCH_ADT_H
#define GRIDPATCH_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridPatch_adt.h
 * @ingroup libgridPatch
 * @brief  Provides the implemenation of the main structure of a grid patch.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdint.h>
#include "../libutil/varArr.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  Main structure for a grid patch. */
struct gridPatch_struct {
	/** @brief  The lower left corner of the patch. */
	gridPointUint32_t idxLo;
	/** @brief  The size of the patch. */
	gridPointUint32_t dims;
	/** @brief  Stores the number of cells in this patch. */
	uint64_t          numCells;
	/** @brief  Keeps references to the attached variables. */
	varArr_t          vars;
	/** @brief  Keeps reference to the allocated data. */
	varArr_t          varData;
};


#endif
