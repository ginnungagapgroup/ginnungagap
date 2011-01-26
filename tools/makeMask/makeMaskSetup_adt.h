// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKEMASKSETUP_ADT_H
#define MAKEMASKSETUP_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include <stdint.h>


/*--- Implemention of main structure ------------------------------------*/
struct makeMaskSetup_struct {
	uint32_t baseGridSize1D;
	uint32_t numLevels;
	uint32_t refinementFactor;
	char     *outFileName;
};


#endif
