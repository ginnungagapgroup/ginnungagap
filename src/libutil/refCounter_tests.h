// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef REFCOUNTER_TESTS_H
#define REFCOUNTER_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
refCounter_init_test(void);

extern bool
refCounter_ref_test(void);

extern bool
refCounter_deref_test(void);

extern bool
refCounter_noReferenceLeft_test(void);



#endif
