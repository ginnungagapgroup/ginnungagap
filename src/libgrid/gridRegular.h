// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_H
#define GRIDREGULAR_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridRegular.h
 * @ingroup libgridRegular
 * @brief  This file provides the interface to regular grids.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "gridPatch.h"
#include "../libdata/dataVar.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridRegular_struct *gridRegular_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 * @{
 */

/**
 * @brief  Creates a new grid with the provided values.
 *
 * @param[in]  *name
 *                The name of the grid.  Must be a properly terminated
 *                string.  Passing @c NULL is undefined.
 * @param[in]  origin
 *                The (physical) origin of the grid.
 * @param[in]  extent
 *                The (physical) extent of the grind.
 * @param[in]  dims
 *                The dimensions of the grid.
 *
 * @return  Returns a new (empty) grid with the provided meta data.
 */
extern gridRegular_t
gridRegular_new(const char        *name,
                gridPointDbl_t    origin,
                gridPointDbl_t    extent,
                gridPointUint32_t dims);

/**
 * @brief  Makes a copy of a grid (only meta data).
 *
 * @param[in]  grid
 *                The grid that should be copied.
 *
 * @return  Returns a new grid that has the same meta-data as the original
 *          grid, but is otherwise empty (no variables attached).
 */
extern gridRegular_t
gridRegular_cloneWithoutData(const gridRegular_t grid);

/**
 * @brief  Dereferences a grid.
 *
 * This will free all associated memory when the last reference to the grid
 * is deleted.
 *
 * @param[in,out]  *gridRegular
 *                    A pointer to the external variable holding the grid
 *                    that should be deleted.  After completion the external
 *                    reference is set to @c NULL.  Passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridRegular_del(gridRegular_t *gridRegular);

/**
 * @brief  Creates a reference to a grid.
 *
 * @param[in,out]  grid
 *                    The grid to generate a reference of.  Passing @c NULL
 *                    is undefined.
 *
 * @return  Returns a new grid reference.
 */
extern gridRegular_t
gridRegular_getRef(gridRegular_t grid);


/** @} */

/**
 * @name Getting
 * @{
 */

extern char *
gridRegular_getName(gridRegular_t grid);

extern void
gridRegular_getOrigin(gridRegular_t grid, gridPointDbl_t origin);

extern void
gridRegular_getExtent(gridRegular_t grid, gridPointDbl_t extent);

extern void
gridRegular_getDelta(gridRegular_t grid, gridPointDbl_t delta);

extern void
gridRegular_getDims(gridRegular_t grid, gridPointUint32_t dims);

extern void
gridRegular_getDimsComplex(gridRegular_t     grid,
                           gridPointUint32_t dimsComplex);

extern void
gridRegular_getPermute(gridRegular_t grid, gridPointInt_t permute);

extern int
gridRegular_getNumVars(gridRegular_t grid);

extern uint64_t
gridRegular_getNumCellsTotal(gridRegular_t grid);


/** @} */

/**
 * @name Setting
 * @{
 */

extern void
gridRegular_setComplexified(gridRegular_t grid, int idxOfVar);

extern void
gridRegular_unsetComplexified(gridRegular_t grid, int idxOfVar);


/** @} */

/**
 * @name Queries
 * @{
 */

extern bool
gridRegular_isComplexified(gridRegular_t grid, int idxOfVar);


/** @} */

/**
 * @name Working with Variables
 * @{
 */

extern int
gridRegular_attachVar(gridRegular_t grid, dataVar_t var);

extern dataVar_t
gridRegular_detachVar(gridRegular_t grid, int idxOfVar);

extern dataVar_t
gridRegular_getVarHandle(gridRegular_t grid, int idxOfVar);

extern void
gridRegular_freeVarData(gridRegular_t grid, int idxOfVarData);


/** @} */

/**
 * @name Working with Patches
 * @{
 */


extern int
gridRegular_getNumPatches(gridRegular_t grid);

extern int
gridRegular_attachPatch(gridRegular_t grid, gridPatch_t patch);

extern gridPatch_t
gridRegular_detachPatch(gridRegular_t grid, int idxOfPatch);

extern gridPatch_t
gridRegular_getPatchHandle(gridRegular_t grid, int idxPatchToGet);

extern void
gridRegular_replacePatch(gridRegular_t grid,
                         int           idxOfPatch,
                         gridPatch_t   newPatch);


/** @} */

/**
 * @name Using
 * @{
 */

extern void
gridRegular_transpose(gridRegular_t grid, int dimA, int dimB);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridRegular Regular Grids
 * @ingroup libgrid
 * @brief This provides regular grids.
 */


#endif
