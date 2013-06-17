// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASK_TESTS_H
#define G9PMASK_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMask_tests.h
 * @ingroup  libg9pMaskTest
 * @brief  Tests for @ref libg9pMask.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
g9pMask_verifyCreationOfMinMaxMask(void);

extern bool
g9pMask_verifyMaxNumCells(void);

extern bool
g9pMask_verifyNumCellsEmptyMask(void);

extern bool
g9pMask_verifyCreationOfGridStructure(void);

extern bool
g9pMask_verifyCreationOfPatch(void);

/**
 * @brief  Verifies that referencing/dereferencing works as expected.
 *
 * @return  Returns @c true if the test passed and @c false otherwise.
 */
extern bool
g9pMask_verifyDelete(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @libg9pMaskTest Mask Tests
 * @ingroup libg9pMask
 * @brief  Provides tests for the mask object.
 */

#endif
