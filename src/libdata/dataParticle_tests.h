// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARTDESC_TESTS_H
#define PARTDESC_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
partDesc_new_test(void);

extern bool
partDesc_getRef_test(void);

extern bool
partDesc_del_test(void);

extern bool
partDesc_addVar_test(void);

extern bool
partDesc_lock_test(void);

extern bool
partDesc_isLocked_test(void);

extern bool
partDesc_getNameHandle_test(void);

extern bool
partDesc_getPartDescID_test(void);

extern bool
partDesc_getNumVars_test(void);

extern bool
partDesc_getVarHandle_test(void);


#endif
