// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVAR_H
#define GRIDVAR_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarTypes.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridVar_struct *gridVar_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridVar_t
gridVar_new(const char *varName, gridVarTypes_t type);

extern void
gridVar_del(gridVar_t *var);

extern void
gridVar_realloc(gridVar_t var, size_t numElements);

extern void
gridVar_free(gridVar_t var);


#endif
