// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef TILE_TESTS_H
#define TILE_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/tile_tests.h
 * @ingroup  libutilMiscTile
 * @brief  Provides the interface to the test functions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  This will test tile_calcIdxsELAE().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcIdxsELAE_test(void);

/**
 * @brief  This will test tile_calcTileNumberForIdxELAE().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcTileNumberForIdxELAE_test(void);

/**
 * @brief  This will test tile_calcIdxsELAB().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcIdxsELAB_test(void);

/**
 * @brief  This will test tile_calcTileNumberForIdxELAB().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcTileNumberForIdxELAB_test(void);

/**
 * @brief  This will test tile_calcNumLargeTilesEven().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcNumLargeTilesEven_test(void);

/**
 * @brief  This will test tile_calcNumSmallTilesEven().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcNumSmallTilesEven_test(void);

/**
 * @brief  This will test tile_calcMinTileSizeEven().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcMinTileSizeEven_test(void);

/**
 * @brief  This will test tile_calcMaxTileSizeEven().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
tile_calcMaxTileSizeEven_test(void);



#endif
