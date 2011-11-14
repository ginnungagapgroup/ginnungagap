// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_TESTS_H
#define GRIDREGULAR_TESTS_H

/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridRegular_tests.h
 * @ingroup libgridRegular
 * @brief  This file provides the interface for the tests of gridRegular.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  This will test gridRegular_new().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_new_test(void);

/**
 * @brief  This will test gridRegular_del().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_del_test(void);

/**
 * @brief  This will test gridRegular_getRef().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getRef_test(void);

/**
 * @brief  This will test gridRegular_getName().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getName_test(void);

/**
 * @brief  This will test gridRegular_getOrigin().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getOrigin_test(void);

/**
 * @brief  This will test gridRegular_getExtent().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getExtent_test(void);

/**
 * @brief  This will test gridRegular_getDelta().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getDelta_test(void);

/**
 * @brief  This will test gridRegular_getDims().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getDims_test(void);

/**
 * @brief  This will test gridRegular_getDimsComplex().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getDimsComplex_test(void);

/**
 * @brief  This will test gridRegular_getPermute().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getPermute_test(void);

/**
 * @brief  This will test gridRegular_getNumVars().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getNumVars_test(void);

/**
 * @brief  This will test gridRegular_attachVar().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_attachVar_test(void);

/**
 * @brief  This will test gridRegular_detachVar().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_detachVar_test(void);

/**
 * @brief  This will test gridRegular_getVarHandle().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getVarHandle_test(void);

/**
 * @brief  This will test gridRegular_getNumPatches().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getNumPatches_test(void);

/**
 * @brief  This will test gridRegular_getNumCellsTotal().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getNumCellsTotal_test(void);

/**
 * @brief  This will test gridRegular_attachPatch().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_attachPatch_test(void);

/**
 * @brief  This will test gridRegular_detachPatch().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_detachPatch_test(void);

/**
 * @brief  This will test gridRegular_getPatchHandle().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_getPatchHandle_test(void);

/**
 * @brief  This will test gridRegular_replacePatch().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_replacePatch_test(void);

/**
 * @brief  This will test gridRegular_transpose().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridRegular_transpose_test(void);


#endif
