// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAVAR_ADT_H
#define DATAVAR_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataVarType.h"
#include "../libutil/refCounter.h"
#include <stdlib.h>
#include <stdbool.h>


/*--- ADT implementation ------------------------------------------------*/
struct dataVar_struct {
	refCounter_t  refCounter;
	char          *name;
	dataVarType_t type;
	int           numComponents;
	void          *(*mallocFunc)(size_t size);
	void          (*freeFunc)(void *ptr);
	bool          isFFTWPadded;
	bool          isComplexified;
};


#endif
