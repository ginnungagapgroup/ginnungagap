// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef RNG_ADT_H
#define RNG_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/rng_adt.h
 * @ingroup libutilFilesMisc
 * @brief  This file provides the implementation of the main structure
 *         of the RNG.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "rng.h"


/*--- ADT implementation ------------------------------------------------*/
struct rng_struct {
	int **streams;
	int generatorType;
	int baseStreamId;
	int numStreamsTotal;
	int numStreamsLocal;
	int randomSeed;
};

#endif
