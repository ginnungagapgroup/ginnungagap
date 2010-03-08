// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef VARARR_ADT_H
#define VARARR_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "varArr.h"


/*--- ADT implementation ------------------------------------------------*/
struct varArr_struct {
	int numAllocated;
	int numUsed;
	int increment;
	void **elements;
};


#endif
