// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDHISTOGRAM_TESTS_H
#define GRIDHISTOGRAM_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridHistogram_tests.h
 * @ingroup libgridAnalysisHisto
 * @brief  This file provides the test functions for grid histograms.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridHistogram_new_test(void);

extern bool
gridHistogram_del_test(void);

extern bool
gridHistogram_calcGridPatch_test(void);

extern bool
gridHistogram_calcGridRegular_test(void);

extern bool
gridHistogram_calcGridRegularDistrib_test(void);


#endif
