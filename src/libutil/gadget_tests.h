// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGET_TESTS_H
#define GADGET_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/gadget_tests.h
 * @ingroup  libutilFilesGadgetTests
 * @brief  Provides the interface for the Gadget tests.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/** @brief  Tests gadget_new(). */
extern bool
gadget_new_test(void);

/** @brief  Tests gadget_newSimple(). */
extern bool
gadget_newSimple_test(void);

/** @brief  Tests gadget_del(). */
extern bool
gadget_del_test(void);

/** @brief  Tests gadget_writeHeaderToCurrentFile(). */
extern bool
gadget_writeHeaderToCurrentFile_test(void);

/** @brief  Tests gadget_writeHeaderToCurrentFile(). */
extern bool
gadget_writeBlockToCurrentFile_test(void);


/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetTests Tests
 * @ingroup libutilFilesGadget
 * @brief Provides tests for @ref libutilFilesGadget.
 */

#endif
