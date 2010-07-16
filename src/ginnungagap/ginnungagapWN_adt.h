// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPWN_ADT_H
#define GINNUNGAGAPWN_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include <stdbool.h>
#include "../libgrid/gridReader.h"
#include "../libgrid/gridWriter.h"
#include "../libutil/rng.h"


/*--- ADT implementation ------------------------------------------------*/
struct ginnungagapWN_struct {
	bool         useFile;
	gridReader_t reader;
	rng_t        rng;
	bool         dumpWhiteNoise;
	gridWriter_t writer;
};


#endif
