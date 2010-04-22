// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEMEBUFFER_TESTS_H
#define COMMSCHEMEBUFFER_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#ifdef WITH_MPI
#  include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
commSchemeBuffer_new_test(void);

extern bool
commSchemeBuffer_del_test(void);

extern bool
commSchemeBuffer_getBuf_test(void);

extern bool
commSchemeBuffer_getCount_test(void);

extern bool
commSchemeBuffer_getDatatype_test(void);

extern bool
commSchemeBuffer_getRank_test(void);

#endif
#endif
