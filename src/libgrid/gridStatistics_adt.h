// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDSTATISTICS_ADT_H
#define GRIDSTATISTICS_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- ADT implementation ------------------------------------------------*/
struct gridStatistics_struct {
	bool   valid;
	double mean;
	double var;
	double skew;
	double kurt;
	double min;
	double max;
};


#endif
