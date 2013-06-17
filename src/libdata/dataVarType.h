// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAVARTYPE_H
#define DATAVARTYPE_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include <stdlib.h>
#include <stdbool.h>


/*--- Exported types ----------------------------------------------------*/
typedef enum {
	DATAVARTYPE_INT8,
	DATAVARTYPE_INT,
	DATAVARTYPE_INT32,
	DATAVARTYPE_INT64,
	DATAVARTYPE_DOUBLE,
	DATAVARTYPE_FLOAT,
	DATAVARTYPE_FPV
} dataVarType_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern size_t
dataVarType_sizeof(dataVarType_t type);

extern bool
dataVarType_isFloating(dataVarType_t type);

extern bool
dataVarType_isInteger(dataVarType_t type);

extern bool
dataVarType_isNativeFloat(dataVarType_t type);

extern bool
dataVarType_isNativeDouble(dataVarType_t type);


#endif
