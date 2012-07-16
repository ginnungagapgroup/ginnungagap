// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASKSHAPELET_ADT_H
#define G9PMASKSHAPELET_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMaskShapelet_adt.h
 * @ingroup  libg9pMaskShapelet
 * @brief  The main structure of the shapelet.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdint.h>


/*--- ADT implementation ------------------------------------------------*/
struct g9pMaskShapelet_struct {
	int8_t low;
	int8_t high;
	int8_t extent;
	int8_t dim1D;
	int8_t *data;
};


#endif
