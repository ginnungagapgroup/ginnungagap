// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#ifndef COSMOPK_ADT_H
#define COSMOPK_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libcosmo/cosmoPk_adt.h
 * @ingroup libcosmo libcosmoPowerspectrum
 * @brief  This file implements the main power spectrum structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdint.h>
#include <gsl/gsl_spline.h>


/*--- Implemention of the ADT structure ---------------------------------*/
struct cosmoPk_struct {
	/** @brief An array of length #numPoints holding the frequencies. */
	uint32_t         numPoints;
	/** @brief An array of length #numPoints holding the frequencies. */
	double           *k;
	/** @brief An array of length #numPoints holding the power. */
	double           *P;
	/** @brief The slope of the PS beyond the highest frequency. */
	double           slopeBeyondKmax;
	/** @brief The slope of the PS below the lowest frequency. */
	double           slopeBeforeKmin;
	/** @brief Stores the accelerator for the spline interpolation. */
	gsl_interp_accel *acc;
	/** @brief Stores the spline interpolation. */
	gsl_spline       *spline;
};


#endif
