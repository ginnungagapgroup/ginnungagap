// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPATCH_TESTS_H
#define GRIDPATCH_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridPatch_new_test(void);

extern bool
gridPatch_del_test(void);

extern bool
gridPatch_getOneDim_test(void);

extern bool
gridPatch_getNumCells_test(void);

extern bool
gridPatch_getIdxLo_test(void);

extern bool
gridPatch_attachVarData_test(void);

extern bool
gridPatch_detachVarData_test(void);

extern bool
gridPatch_getVarHandle_test(void);

extern bool
gridPatch_getVarDataHandle_test(void);

extern bool
gridPatch_getNumVars_test(void);

#endif
