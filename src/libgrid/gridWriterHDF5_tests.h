// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERHDF5_TESTS_H
#define GRIDWRITERHDF5_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterHDF5_tests.h
 * @ingroup  libgridIOOutHDF5
 * @brief  Provides the interface for testing gridWriterHDF5.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridWriterHDF5_new_test(void);

extern bool
gridWriterHDF5_del_test(void);


#endif
