// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
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
	/** @brief  The name of the file on disk. */
	char     *graficFileName;
	/** @brief  The endianess of the machine. */
	endian_t machineEndianess;
	/** @brief  The endianess of the file. */
	endian_t fileEndianess;
	/** @brief  Toggles whether the file is a white noise file. */
	bool     isWhiteNoise;
	/** @brief  Gives the size of the header. */
	int      headerSkip;
	// Header entries always there
	/** @brief  The x-size of the grid. */
	uint32_t np1;
	/** @brief  The y-size of the grid. */
	uint32_t np2;
	/** @brief  The z-size of the grid. */
	uint32_t np3;
	// Header entries only when normal file
	/** @brief  The cell spacing in @c Mpc. */
	float dx;
	/** @brief  The offset in x. */
	float x1o;
	/** @brief  The offset in y. */
	float x2o;
	/** @brief  The offset in z. */
	float x3o;
	/** @brief  The expansion paramter @c a. */
	float astart;
	/** @brief  The matter density in units of the critical density. */
	float omegam;
	/** @brief  The dark energy density in units of the critical density. */
	float omegav;
	/** @brief  The Hubble parameters in units of @c km/s/Mpc. */
	float h0;
	// Header entries only when white noise file
	/** @brief  The random number seed.. */
	int iseed;
};


#endif
