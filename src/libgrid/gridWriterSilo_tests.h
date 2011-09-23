// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERSILO_TESTS_H
#define GRIDWRITERSILO_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridWriterSilo_new_test(void);

extern bool
gridWriterSilo_del_test(void);

extern bool
gridWriterSilo_activate_test(void);

extern bool
gridWriterSilo_deactivate_test(void);

extern bool
gridWriterSilo_writeGridPatch_test(void);

extern bool
gridWriterSilo_writeGridRegular_test(void);


#ifdef WITH_MPI
extern bool
gridWriterSilo_initParallel_test(void);

#endif


#endif
