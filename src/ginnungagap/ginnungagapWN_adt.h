// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPWN_ADT_H
#define GINNUNGAGAPWN_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include <stdbool.h>
#ifdef WITH_SPRNG
#  include "../libutil/rng.h"
#endif


/*--- ADT implementation ------------------------------------------------*/
struct ginnungagapWN_struct {
	bool  useFile;
#ifdef WITH_SPRNG
	rng_t rng;
#endif
	char *whiteNoiseFileName;
};


#endif
