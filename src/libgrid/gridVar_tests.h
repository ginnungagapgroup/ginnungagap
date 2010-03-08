// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVAR_TESTS_H
#define GRIDVAR_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridVar_new_test(void);

extern bool
gridVar_del_test(void);

extern bool
gridVar_realloc_test(void);

extern bool
gridVar_free_test(void);


#endif
