// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_ADT_H
#define GRIDREGULAR_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "../libutil/refCounter.h"
#include "../libutil/varArr.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridRegular_struct {
	refCounter_t      refCounter;
	char              *name;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointDbl_t    delta;
	gridPointUint32_t dims;
	varArr_t          patches;
	varArr_t          vars;
};


#endif
