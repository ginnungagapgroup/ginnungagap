// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef LIDX_TESTS_H
#define LIDX_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  lIdx_tests.h
 * @ingroup  libutilMis
 * @brief  Provides the interface for testing the lIdx module.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Tests lIdx_fromCoord2d().
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
lIdx_fromCoord2d_test(void);

/**
 * @brief  Tests lIdx_toCoord2d().
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
lIdx_toCoord2d_test(void);

/**
 * @brief  Tests lIdx_fromCoord3d().
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
lIdx_fromCoord3d_test(void);

/**
 * @brief  Tests lIdx_toCoord3d().
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
lIdx_toCoord3d_test(void);

/**
 * @brief  Tests lIdx_fromCoordNd().
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
lIdx_fromCoordNd_test(void);

/**
 * @brief  Tests lIdx_toCoordNd().
 *
 * @return  Returns @c true if the tests succeeded and @c false otherwise.
 */
extern bool
lIdx_toCoordNd_test(void);


#endif
