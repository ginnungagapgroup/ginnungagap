// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LARE_ADT_H
#define LARE_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include <stdint.h>
#include "../libgrid/gridPoint.h"


/*--- ADT implementation ------------------------------------------------*/
struct lare_struct {
	uint32_t          numElements;
	gridPointUint32_t dims;
	gridPointUint32_t *elements;
};


#endif
