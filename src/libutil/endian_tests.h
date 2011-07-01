// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ENDIAN_TESTS_H
#define ENDIAN_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/endian_tests.h
 * @ingroup  libutilMiscEndian
 * @brief  This provides the prototypes of the test functions for
 *         endian.c
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  This will test endian_getFileEndianessByBlock().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
endian_getFileEndianessByBlock_test(void);

/**
 * @brief  This will test endian_getFileEndianessByBlockF().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
endian_getFileEndianessByBlockF_test(void);

/**
 * @brief  This will test endian_fileIsLittleByBlock().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
endian_fileIsLittleByBlock_test(void);

/**
 * @brief  This will test endian_fileIsBigByBlock().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
endian_fileIsBigByBlock_test(void);

/**
 * @brief  This will test endian_fileIsLittleByBlockF().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
endian_fileIsLittleByBlockF_test(void);

/**
 * @brief  This will test endian_fileIsBigByBlockF().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
endian_fileIsBigByBlockF_test(void);

#endif
