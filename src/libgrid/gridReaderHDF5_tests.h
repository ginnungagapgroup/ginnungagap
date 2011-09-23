// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERHDF5_TESTS_H
#define GRIDREADERHDF5_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5_tests.h
 * @ingroup  libgridIOInHDF5
 * @brief  Provides the interface to the test for gridReaderHDF5.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridReaderHDF5_newFromIni_test(void);

extern bool
gridReaderHDF5_del_test(void);

extern bool
gridReaderHDF5_readIntoPatch_test(void);

extern bool
gridReaderHDF5_readIntoPatchForVar_test(void);

#endif
