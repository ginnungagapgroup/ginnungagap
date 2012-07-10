// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef HIERARCHY_TESTS_H
#define HIERARCHY_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchy_tests.h
 * @ingroup  libg9pHierarchyTest
 * @brief  Tests for @ref libg9pHierarchy.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
g9pHierarchy_verifyComplexCreationMethodsAreIdentical(void);

extern bool
g9pHierarchy_verifySimpleCreationMethodsAreIdentical(void);

extern bool
g9pHierarchy_verifyDimsSelections(void);

extern bool
g9pHierarchy_verifyFactorSelections(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @libg9pHierarchyTest Hierarchy Tests
 * @ingroup libg9pHierarchy
 * @brief  Provides tests for the hierarchy object.
 */

#endif
