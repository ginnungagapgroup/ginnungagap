// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKEMASK_ADT_H
#define MAKEMASK_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "makeMaskSetup.h"
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"


/*--- Implemention of main structure ------------------------------------*/
struct makeMask_struct {
	makeMaskSetup_t      setup;
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
};


#endif
