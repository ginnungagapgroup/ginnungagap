// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADER_TESTS_H
#define GRIDREADER_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridReader_newFromIni_test(void);

extern bool
gridReader_del_test(void);

extern bool
gridReader_readIntoPatch_test(void);

extern bool
gridReader_readIntoPatchForVar_test(void);


#endif
