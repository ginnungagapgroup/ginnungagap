// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef CUBEPM_TESTS_H
#define CUBEPM_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/cubepm_tests.h
 * @ingroup  libutilFilesCubePM
 * @brief  Provides the interface to the test functions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  This will test cubepm_new().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
cubepm_new_test(void);

/**
 * @brief  This will test cubepm_del().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
cubepm_del_test(void);


#endif
