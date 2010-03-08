// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVARTYPES_H
#define GRIDVARTYPES_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdlib.h>


/*--- Main enum ---------------------------------------------------------*/
typedef enum {
	GRIDVARTYPES_FPV,
	GRIDVARTYPES_FPVVEC,
	GRIDVARTYPES_INT32,
	GRIDVARTYPES_INT32VEC
} gridVarTypes_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern size_t
gridVarTypes_getSizePerElement(gridVarTypes_t type);


#endif
