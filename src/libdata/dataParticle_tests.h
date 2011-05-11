// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAPARTICLE_TESTS_H
#define DATAPARTICLE_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
dataParticle_new_test(void);

extern bool
dataParticle_getRef_test(void);

extern bool
dataParticle_del_test(void);

extern bool
dataParticle_addVar_test(void);

extern bool
dataParticle_lock_test(void);

extern bool
dataParticle_isLocked_test(void);

extern bool
dataParticle_getNameHandle_test(void);

extern bool
dataParticle_getPartDescID_test(void);

extern bool
dataParticle_getNumVars_test(void);

extern bool
dataParticle_getVarHandle_test(void);


#endif
