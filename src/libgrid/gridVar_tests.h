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
gridVar_getRef_test(void);

extern bool
gridVar_getSizePerElement_test(void);

extern bool
gridVar_getNumComponents_test(void);

extern bool
gridVar_getType_test(void);

extern bool
gridVar_getName_test(void);

extern bool
gridVar_setMemFuncs_test(void);

extern bool
gridVar_getMemory_test(void);

extern bool
gridVar_freeMemory_test(void);

extern bool
gridVar_setFFTWPadded_test(void);

extern bool
gridVar_unsetFFTWPadded_test(void);

extern bool
gridVar_isFFTWPadded_test(void);


#endif
