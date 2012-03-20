// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERBOV_TESTS_H
#define GRIDREADERBOV_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderBov_tests.h
 * @ingroup  libgridIOInBovTests
 * @brief  Provides the interface to the tests.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gridReaderBov_new(). */
extern bool
gridReaderBov_new_test(void);


/** @brief  Tests gridReaderBov_del(). */
extern bool
gridReaderBov_del_test(void);


/** @brief  Tests gridReaderBov_getBov(). */
extern bool
gridReaderBov_getBov_test(void);


/** @brief  Tests gridReaderBov_del(). */
extern bool
gridReaderBov_readIntoPatch_test(void);


/** @brief  Tests gridReaderBov_del(). */
extern bool
gridReaderBov_readIntoPatchForVar_test(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInBovTests Tests
 * @ingroup libgridIOInBov
 * @brief  Provides the tests for the BOV reader.
 */


#endif
