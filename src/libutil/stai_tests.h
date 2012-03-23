// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef STAI_TESTS_H
#define STAI_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  stai_tests.h
 * @ingroup  libutilMiscStai
 * @brief  Interface for the test functions of stai.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  This will test stai_new().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_new_test(void);


/**
 * @brief  This will test stai_clone().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_clone_test(void);


/**
 * @brief  This will test stai_cloneWithDifferenBase().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_cloneWithDifferentBase_test(void);


/**
 * @brief  This will test stai_del().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_del_test(void);


/**
 * @brief  This will test stai_setElement().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_setElement_test(void);


/**
 * @brief  This will test stai_setElementsMulti().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_setElementsMulti_test(void);


/**
 * @brief  This will test stai_getElement().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_getElement_test(void);


/**
 * @brief  This will test stai_getElementsMulti().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_getElementsMulti_test(void);

/**
 * @brief  This will test stai_isLinear().
 *
 * @return  Returns true if the test succeeds, false otherwise.
 */
extern bool
stai_isLinear_test(void);


#endif
