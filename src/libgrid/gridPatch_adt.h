// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPATCH_ADT_H
#define GRIDPATCH_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "../libutil/varArr.h"
#include <stdint.h>


/*--- ADT implementation ------------------------------------------------*/
struct gridPatch_struct {
	gridPointUint32_t idxLo;
	gridPointUint32_t dims;
	uint64_t          numCells;
	varArr_t          varData;
};


#endif
