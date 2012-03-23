// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETBLOCK_TESTS_H
#define GADGETBLOCK_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadgetBlock_tests.h
 * @ingroup  libutilFilesGadgetBlockTests
 * @brief  Provides the interface to the Gadget block tests.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gadgetBlock_getNameFromType(). */
extern bool
gadgetBlock_getNameFromType_test(void);


/** @brief  Tests gadgetBlock_getTypeFromName(). */
extern bool
gadgetBlock_getTypeFromName_test(void);


/** @brief  Tests gadgetBlock_getNumComponents(). */
extern bool
gadgetBlock_getNumComponents_test(void);


/** @brief  Tests gadgetBlock_isInteger(). */
extern bool
gadgetBlock_isInteger_test(void);


/** @brief  Tests gadgetBlock_getNumPartsInBlock(). */
extern bool
gadgetBlock_getNumPartsInBlock_test(void);


/**
 * @brief  Tests gadgetBlock_writeDescriptor() and
 *         gadgetBlock_readDescriptor().
 */
extern bool
gadgetBlock_writereadDescriptor_test(void);

/** @brief  Tests gadgetBlock_readDescriptorString(). */
extern bool
gadgetBlock_readDescriptorString_test(void);

/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetBlockTests Tests
 * @ingroup libutilFilesGadgetBlock
 * @brief Provides tests for @ref libutilFilesGadgetBlock.
 */

#endif
