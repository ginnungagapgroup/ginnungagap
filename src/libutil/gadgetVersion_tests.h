// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETVERSION_TESTS_H
#define GADGETVERSION_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetVersion_tests.h
 * @ingroup  libutilFilesGadgetVersionTests
 * @brief  Provides the interface to the Gadget version tests.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gadgetVersion_getVersionFromFile(). */
extern bool
gadgetVersion_getVersionFromFile_test(void);

/** @brief  Tests gadgetVersion_getNameFromType(). */
extern bool
gadgetVersion_getNameFromType_test(void);

/** @brief  Tests gadgetVersion_getTypeFromName(). */
extern bool
gadgetVersion_getTypeFromName_test(void);


/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetVersionTests Tests
 * @ingroup libutilFilesGadgetVersion
 * @brief Provides tests for @ref libutilFilesGadgetVersion.
 */

#endif
