// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#ifndef COSMOPK_ADT_H
#define COSMOPK_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdint.h>
#include <gsl/gsl_spline.h>


/*--- Implemention of the ADT structure ---------------------------------*/
struct cosmoPk_struct {
	uint32_t         numPoints;
	double           *k;
	double           *P;
	double           slopeBeyondKmax;
	double           slopeBeforeKmin;
	gsl_interp_accel *acc;
	gsl_spline       *spline;
};


#endif
