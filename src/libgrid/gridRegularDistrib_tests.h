// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARDISTRIB_TESTS_H
#define GRIDREGULARDISTRIB_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridRegularDistrib_new_test(void);

extern bool
gridRegularDistrib_del_test(void);

extern bool
gridRegularDistrib_getRef_test(void);

#ifdef WITH_MPI
extern bool
gridRegularDistrib_initMPI_test(void);

extern bool
gridRegularDistrib_getLocalRank_test(void);
#endif

extern bool
gridRegularDistrib_getPatchForRank_test(void);

extern bool
gridRegularDistrib_calcIdxsForRank1D_test(void);

extern bool
gridRegularDistrib_transpose_test(void);

#endif
