// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDHISTOGRAM_TESTS_H
#define GRIDHISTOGRAM_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridHistogram_new_test(void);

extern bool
gridHistogram_del_test(void);

extern bool
gridHistogram_calcGridPatch_test(void);

extern bool
gridHistogram_calcGridRegular_test(void);

extern bool
gridHistogram_calcGridRegularDistrib_test(void);


#endif
