// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASKIO_TESTS_H
#define G9PMASKIO_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libg9p/g9pMaskIO_tests.h
 * @ingroup  libg9pMaskIOTest
 * @brief  Provides the inferface for the tests for @ref libg9pMaskIO.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
g9pMaskIO_verifyCreationFromIniExternalHierarchy(void);

extern bool
g9pMaskIO_verifyCreationFromIniWithHierarchyFromIni(void);

extern bool
g9pMaskIO_verifyReadWriteSimple(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pMaskIOTest Mask IO Tests
 * @ingroup libg9pMaskIO
 * @brief Provides tests for the IO routines of @ref libg9pMaskIO.
 */


#endif
