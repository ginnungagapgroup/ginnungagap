// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef VARARR_TESTS_H
#define VARARR_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
varArr_new_test(void);

extern bool
varArr_del_test(void);

extern bool
varArr_getLength_test(void);

extern bool
varArr_insert_test(void);

extern bool
varArr_remove_test(void);

extern bool
varArr_replace_test(void);

extern bool
varArr_getElementHandle_test(void);


#endif
