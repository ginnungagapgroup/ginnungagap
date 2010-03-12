// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarType.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern size_t
gridVarType_sizeof(gridVarType_t type)
{
	size_t size;

	switch (type) {
	default:
	// If type is unknown (should NEVER happen) treat it as an int.
	case GRIDVARTYPE_INT:
		size = sizeof(int);
		break;
	case GRIDVARTYPE_DOUBLE:
		size = sizeof(double);
		break;
	case GRIDVARTYPE_FPV:
		size = sizeof(fpv_t);
		break;
	}

	return size;
}

/*--- Implementations of local functions --------------------------------*/
