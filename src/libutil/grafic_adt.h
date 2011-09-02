// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRAFIC_ADT_H
#define GRAFIC_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/grafic_adt.h
 * @ingroup libutilFilesGrafic
 * @brief  This file provides the main structure of the Grafic ADT.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "grafic.h"
#include <stdbool.h>
#include <stdint.h>
#include "endian.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  Main structure for the Grafic object. */
struct grafic_struct {
	// Handling information
	char     *graficFileName;
	endian_t machineEndianess;
	endian_t fileEndianess;
	bool     isWhiteNoise;
	int      headerSkip;
	// Header entries always there
	uint32_t np1;
	uint32_t np2;
	uint32_t np3;
	// Header entries only when normal file
	float    dx;
	float    x1o;
	float    x2o;
	float    x3o;
	float    astart;
	float    omegam;
	float    omegav;
	float    h0;
	// Header entries only when white noise file
	int      iseed;
};


#endif
