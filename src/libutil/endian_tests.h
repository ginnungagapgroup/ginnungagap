// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ENDIAN_TESTS_H
#define ENDIAN_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
endian_fileIsLittleByBlock_test(void);

extern bool
endian_fileIsBigByBlock_test(void);

#endif
