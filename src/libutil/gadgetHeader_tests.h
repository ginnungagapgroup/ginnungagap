// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETHEADER_TESTS_H
#define GADGETHEADER_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetHeader_tests.h
 * @ingroup  libutilFilesGadgetHeaderTests
 * @brief  Provides the interface to the Gadget header tests.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gadgetHeader_new(). */
extern bool
gadgetHeader_new_test(void);

/** @brief  Tests gadgetHeader_del(). */
extern bool
gadgetHeader_del_test(void);

/** @brief  Tests gadgetHeader_setNall(). */
extern bool
gadgetHeader_setNall_test(void);

/** @brief  Tests gadgetHeader_getNumPartsInFileWithMass(). */
extern bool
gadgetHeader_getNumPartsInFileWithMass_test(void);

/** @brief  Tests gadgetHeader_getNumPartsInBlock(). */
extern bool
gadgetHeader_getNumPartsInBlock_test(void);

/** @brief  Tests gadgetHeader_sizeOfElement(). */
extern bool
gadgetHeader_sizeOfElement_test(void);


/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetHeaderTests Tests
 * @ingroup libutilFilesGadgetHeader
 * @brief Provides tests for @ref libutilFilesGadgetHeader.
 */

#endif
