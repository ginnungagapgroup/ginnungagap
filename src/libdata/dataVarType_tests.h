// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAVARTYPE_TESTS_H
#define DATAVARTYPE_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
dataVarType_sizeof_test(void);

extern bool
dataVarType_isFloating_test(void);

extern bool
dataVarType_isInteger_test(void);

extern bool
dataVarType_isNativeFloat_test(void);

extern bool
dataVarType_isNativeDouble_test(void);


#endif
