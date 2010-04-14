// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVAR_ADT_H
#define GRIDVAR_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarType.h"
#include "../libutil/refCounter.h"
#include <stdlib.h>


/*--- ADT implementation ------------------------------------------------*/
struct gridVar_struct {
	refCounter_t  refCounter;
	char          *name;
	gridVarType_t type;
	int           numComponents;
	void          *(*mallocFunc)(size_t size);
	void          (*freeFunc)(void *ptr);
};


#endif
