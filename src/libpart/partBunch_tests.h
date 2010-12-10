// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARTBUNCH_TESTS_H
#define PARTBUNCH_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include <stdbool.h>
#include <stdint.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
partBunch_new_test(void);

extern bool
partBunch_del_test(void);

extern bool
partBunch_allocMem_test(void);

extern bool
partBunch_freeMem_test(void);

extern bool
partBunch_resize_test(void);

extern bool
partBunch_isAllocated_test(void);

extern bool
partBunch_getNumParticles_test(void);

#endif
