// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERFACTORY_TESTS_H
#define GRIDREADERFACTORY_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderFactory_tests.h
 * @ingroup libgridIOInFactoryTests
 * @brief  This file provides the interface to the testing routines.
 */



/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gridReaderFactory_newReaderFromIni(). */
extern bool
gridReaderFactory_newReaderFromIni_test(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInFactoryTests Tests
 * @ingroup libgridIOInFactory
 * @brief This provides the tests for the grid reader.
 */


#endif
