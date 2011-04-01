// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef VARARR_ADT_H
#define VARARR_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/varArr_adt.h
 * @ingroup libutilMiscVarArr
 * @brief This file provides the implementation of the main structure of
 *        the variable length array.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "varArr.h"


/*--- Defines needed for this ADT ---------------------------------------*/

/** Gives the smallest increment for the changing the array size. */
#define VARARR_MIN_INCR 2


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  Implements the main structure for the variable length array.
 */
struct varArr_struct {
	/** @brief  The number of allocated elements. */
	int  numAllocated;
	/** @brief  The number of used elements. */
	int  numUsed;
	/** @brief  The increment used to resize the array. */
	int  increment;
	/** @brief  The data array holding references to the elements. */
	void **elements;
};


#endif
