// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BOV_TESTS_H
#define BOV_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
bov_new_test(void);

extern bool
bov_newFromFile_test(void);

extern bool
bov_del_test(void);

extern bool
bov_getTime_test(void);

extern bool
bov_getDataFileName_test(void);

extern bool
bov_getDataSize_test(void);

extern bool
bov_getDataFormat_test(void);

extern bool
bov_getVarName_test(void);

extern bool
bov_getDataEndian_test(void);

extern bool
bov_getCentering_test(void);

extern bool
bov_getBrickOrigin_test(void);

extern bool
bov_getBrickSize_test(void);

extern bool
bov_getDataComponents_test(void);

extern bool
bov_setTime_test(void);

extern bool
bov_setDataFileName_test(void);

extern bool
bov_setDataSize_test(void);

extern bool
bov_setDataFormat_test(void);

extern bool
bov_setVarName_test(void);

extern bool
bov_setDataEndian_test(void);

extern bool
bov_setCentering_test(void);

extern bool
bov_setBrickOrigin_test(void);

extern bool
bov_setBrickSize_test(void);

extern bool
bov_setDataComponents_test(void);

extern bool
bov_read_test(void);

extern bool
bov_readWindowed_test(void);


#endif
