// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVARTYPE_H
#define GRIDVARTYPE_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdlib.h>
#include <stdbool.h>


/*--- Exported types ----------------------------------------------------*/
typedef enum {
	GRIDVARTYPE_INT,
	GRIDVARTYPE_DOUBLE,
	GRIDVARTYPE_FPV
} gridVarType_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern size_t
gridVarType_sizeof(gridVarType_t type);

extern bool
gridVarType_isFloating(gridVarType_t type);

extern bool
gridVarType_isInteger(gridVarType_t type);

extern bool
gridVarType_isNativeFloat(gridVarType_t type);

extern bool
gridVarType_isNativeDouble(gridVarType_t type);


#endif
