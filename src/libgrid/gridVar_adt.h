// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVAR_ADT_H
#define GRIDVAR_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarTypes.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridVar_struct {
	char           *name;
	gridVarTypes_t type;
	size_t         numElements;
	size_t         sizePerElement;
	void           *data;
};


#endif
