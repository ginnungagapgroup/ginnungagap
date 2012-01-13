// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef FILENAME_TESTS_H
#define FILENAME_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/filename_tests.h
 * @ingroup  libutilMiscFilenameTest
 * @brief  Provides the interface to testing the filename object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Tests filename_new();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_new_test(void);

/**
 * @brief  Tests filename_newFull();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_newFull_test(void);

/**
 * @brief  Tests filename_del();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_del_test(void);

/**
 * @brief  Tests filename_setPath();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_setPath_test(void);

/**
 * @brief  Tests filename_setPrefix();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_setPrefix_test(void);

/**
 * @brief  Tests filename_setQualifier();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_setQualifier_test(void);

/**
 * @brief  Tests filename_setSuffix();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_setSuffix_test(void);

/**
 * @brief  Tests filename_getFullName();
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
filename_getFullName_test(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilMiscFilenameTest  Tests for the Filename Object
 * @ingroup libutilMiscFilename
 * @brief Provides the tests for the filename object.
 */


#endif
