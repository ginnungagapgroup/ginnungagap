// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDSTATISTICS_TESTS_H
#define GRIDSTATISTICS_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridStatistics_new_test(void);

extern bool
gridStatistics_del_test(void);

extern bool
gridStatistics_calcGridPatch_test(void);

extern bool
gridStatistics_calcGridRegular_test(void);

extern bool
gridStatistics_calcGridRegularDistrib_test(void);

extern bool
gridStatistics_invalidate_test(void);

extern bool
gridStatistics_isValid_test(void);

extern bool
gridStatistics_getMean_test(void);

extern bool
gridStatistics_getSD_test(void);

extern bool
gridStatistics_getVariance_test(void);

extern bool
gridStatistics_getSkew_test(void);

extern bool
gridStatistics_getKurtosis_test(void);

extern bool
gridStatistics_getMin_test(void);

extern bool
gridStatistics_getMax_test(void);


#endif
