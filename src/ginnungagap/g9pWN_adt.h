// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PWN_ADT_H
#define G9PWN_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pWN_adt.h
 * @ingroup  ginnungagapWN
 * @brief  Provides the implementation of the WN structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdbool.h>
#include "../libgrid/gridReader.h"
#include "../libgrid/gridWriter.h"
#include "../libutil/rng.h"


/*--- ADT implementation ------------------------------------------------*/
struct g9pWN_struct {
	bool         useFile;
	gridReader_t reader;
	rng_t        rng;
	bool         dumpWhiteNoise;
	gridWriter_t writer;
};


#endif
