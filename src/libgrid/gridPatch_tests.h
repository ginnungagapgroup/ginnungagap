// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPATCH_TESTS_H
#define GRIDPATCH_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridPatch_tests.h
 * @ingroup libgridPatch
 * @brief  This file provides the interface for the tests of gridPatch.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  This will test gridPatch_new().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_new_test(void);

/**
 * @brief  This will test gridPatch_del().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_del_test(void);

/**
 * @brief  This will test gridPatch_getOneDim().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getOneDim_test(void);

/**
 * @brief  This will test gridPatch_getDimActual1D().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getDimActual1D_test(void);

/**
 * @brief  This will test gridPatch_getDims().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getDims_test(void);

/**
 * @brief  This will test gridPatch_getDimsActual().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getDimsActual_test(void);

/**
 * @brief  This will test gridPatch_getNumCells().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getNumCells_test(void);

/**
 * @brief  This will test gridPatch_getNumCellsActual().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getNumCellsActual_test(void);

/**
 * @brief  This will test gridPatch_getIdxLo().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getIdxLo_test(void);

/**
 * @brief  This will test gridPatch_attachVar().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_attachVar_test(void);

/**
 * @brief  This will test gridPatch_detachVar().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_detachVar_test(void);

/**
 * @brief  This will test gridPatch_allocateVarData().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_allocateVarData_test(void);

/**
 * @brief  This will test gridPatch_freeVarData().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_freeVarData_test(void);

/**
 * @brief  This will test gridPatch_replaceVarData().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_replaceVarData_test(void);

/**
 * @brief  This will test gridPatch_popVarData().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_popVarData_test(void);

/**
 * @brief  This will test gridPatch_getVarHandle().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getVarHandle_test(void);

/**
 * @brief  This will test gridPatch_getVarDataHandle().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getVarDataHandle_test(void);

/**
 * @brief  This will test gridPatch_getVarDataHandleByVar().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getVarDataHandleByVar_test(void);

/**
 * @brief  This will test gridPatch_getNumVars().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getNumVars_test(void);

/**
 * @brief  This will test gridPatch_transpose().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_transpose_test(void);

/**
 * @brief  This will test gridPatch_getWindowedDataCopy().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_getWindowedDataCopy_test(void);

/**
 * @brief  This will test gridPatch_putWindowedData().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_putWindowedData_test(void);

/**
 * @brief  This will test gridPatch_calcDistanceVector().
 *
 * @return  Returns @c true if the test succeeded and @c false
 *          otherwise.
 */
extern bool
gridPatch_calcDistanceVector_test(void);

#endif
