// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PHIERARCHY_ADT_H
#define G9PHIERARCHY_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchy_adt.h
 * @ingroup  libg9pHierarchy
 * @brief  Provides the implementation of the hierarchy structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdint.h>


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main hierarchy structure. */
struct g9pHierarchy_struct {
	/// @brief Number of levels.
	uint8_t  numLevels;
	/// @breif The resolutions of each level.
	uint32_t *dim1Ds;
	/// @brief Stores the factors between the levels.
	uint32_t *factors;
};


#endif
