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
gridPatch_getDimActual1D_test(void);

extern bool
gridPatch_getDims_test(void);

extern bool
gridPatch_getDimsActual_test(void);

extern bool
gridPatch_getNumCells_test(void);

extern bool
gridPatch_getNumCellsActual_test(void);

extern bool
gridPatch_getIdxLo_test(void);

extern bool
gridPatch_attachVar_test(void);

extern bool
gridPatch_detachVar_test(void);

extern bool
gridPatch_allocateVarData_test(void);

extern bool
gridPatch_freeVarData_test(void);

extern bool
gridPatch_replaceVarData_test(void);

extern bool
gridPatch_getVarHandle_test(void);

extern bool
gridPatch_getVarDataHandle_test(void);

extern bool
gridPatch_getNumVars_test(void);

extern bool
gridPatch_transpose_test(void);

extern bool
gridPatch_getWindowedDataCopy_test(void);

extern bool
gridPatch_putWindowedData_test(void);


#endif
