// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_ADT_H
#define GRIDREGULAR_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridRegular_adt.h
 * @ingroup libgridRegular
 * @brief  Provides the implemenation of the main structure of a regular
 *         grid.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "../libutil/refCounter.h"
#include "../libutil/varArr.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  Main structures for a regular grid. */
struct gridRegular_struct {
	/** @brief  Keeps track of the reference to the grid. */
	refCounter_t      refCounter;
	/** @brief  The name of the grid. */
	char              *name;
	/** @brief  Stores the (physical) origin of the grid. */
	gridPointDbl_t    origin;
	/** @brief  Stores the (physical) extent of the grid. */
	gridPointDbl_t    extent;
	/** @brief  Stores the spacing of the grid. */
	gridPointDbl_t    delta;
	/** @brief  Stores the dimensions of the grid. */
	gridPointUint32_t dims;
	/** @brief  Stores the complex dimensions of the grid. */
	gridPointUint32_t dimsComplex;
	/** @brief  Keeps track of permuations of the dimensions. */
	gridPointInt_t    permute;
	/** @brief  Stores the patches attached to the grid. */
	varArr_t          patches;
	/** @brief  Stores the variables handled by the grid. */
	varArr_t          vars;
};


#endif
