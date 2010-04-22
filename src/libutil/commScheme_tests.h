// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEME_TESTS_H
#define COMMSCHEME_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
commScheme_new_test(void);

extern bool
commScheme_del_test(void);

extern bool
commScheme_addBuffer_test(void);

extern bool
commScheme_execute_test(void);

extern bool
commScheme_executeBlock_test(void);

extern bool
commScheme_wait_test(void);


#endif
