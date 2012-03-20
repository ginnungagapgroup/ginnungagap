// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERHDF5_TESTS_H
#define GRIDREADERHDF5_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5_tests.h
 * @ingroup  libgridIOInHDF5Tests
 * @brief  Provides the interface to the test for gridReaderHDF5.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gridReaderHDF5_new(). */
extern bool
gridReaderHDF5_new_test(void);


/** @brief  Tests gridReaderHDF5_del(). */
extern bool
gridReaderHDF5_del_test(void);


/** @brief  Tests gridReaderHDF5_getH5File(). */
extern bool
gridReaderHDF5_getH5File_test(void);


/** @brief  Tests gridReaderHDF5_readIntoPatch(). */
extern bool
gridReaderHDF5_readIntoPatch_test(void);


/** @brief  Tests gridReaderHDF5_readIntoPatchForVar(). */
extern bool
gridReaderHDF5_readIntoPatchForVar_test(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInHDF5Tests Tests
 * @ingroup libgridIOInHDF5
 * @brief  Provides the tests for the HDF5 reader.
 */

#endif
