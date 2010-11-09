// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARTBUNCH_ADT_H
#define PARTBUNCH_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include "partDesc.h"
#include <stdint.h>
#include <stdbool.h>
#include "../libutil/varArr.h"


/*--- ADT implementation ------------------------------------------------*/
struct partBunch_struct {
	partDesc_t desc;
	uint64_t   numParticles;
	varArr_t   data;
	bool       isAllocated;
};


#endif
