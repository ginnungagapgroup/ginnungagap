// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARTDESC_ADT_H
#define PARTDESC_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include "partDesc.h"
#include <stdbool.h>
#include "../libutil/refCounter.h"
#include "../libutil/varArr.h"


/*--- ADT implementation ------------------------------------------------*/
struct partDesc_struct {
	refCounter_t refCounter;
	char         *name;
	int          partDescID;
	varArr_t     vars;
	bool         isLocked;
};


#endif
