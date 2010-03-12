// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVAR_ADT_H
#define GRIDVAR_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarType.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridVar_struct {
	char          *name;
	gridVarType_t type;
	int           numComponents;
};


#endif
