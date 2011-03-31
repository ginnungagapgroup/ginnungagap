// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef VARARR_ADT_H
#define VARARR_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/varArr_adt.h
 * @ingroup libutilMisc
 * @brief This file provides the implementation of the main structure of
 *        the variable length array.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "varArr.h"


/*--- Defines needed for this ADT ---------------------------------------*/
#define VARARR_MIN_INCR 2


/*--- ADT implementation ------------------------------------------------*/
struct varArr_struct {
	int numAllocated;
	int numUsed;
	int increment;
	void **elements;
};


#endif
