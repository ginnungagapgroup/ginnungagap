// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAVAR_TESTS_H
#define DATAVAR_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
dataVar_new_test(void);

extern bool
dataVar_clone_test(void);

extern bool
dataVar_del_test(void);

extern bool
dataVar_getRef_test(void);

extern bool
dataVar_getSizePerElement_test(void);

extern bool
dataVar_getNumComponents_test(void);

extern bool
dataVar_getType_test(void);

extern bool
dataVar_getName_test(void);

extern bool
dataVar_setMemFuncs_test(void);

extern bool
dataVar_getMemory_test(void);

extern bool
dataVar_freeMemory_test(void);

extern bool
dataVar_getPointerByOffset_test(void);

extern bool
dataVar_setFFTWPadded_test(void);

extern bool
dataVar_unsetFFTWPadded_test(void);

extern bool
dataVar_isFFTWPadded_test(void);

extern bool
dataVar_setComplexified_test(void);

extern bool
dataVar_unsetComplexified_test(void);

extern bool
dataVar_isComplexified_test(void);

#ifdef WITH_MPI

extern bool
dataVar_getMPIDatatype_test(void);

extern bool
dataVar_getMPICount_test(void);

#endif

#endif
