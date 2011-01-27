// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDHISTOGRAM_ADT_H
#define GRIDHISTOGRAM_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridHistogram_struct {
	uint32_t numBins;
	uint32_t numBinsReal;
	double   *binLimits;
	uint32_t *binCounts;
	uint64_t totalCounts;
	uint64_t totalCountsInRange;
};


#endif
