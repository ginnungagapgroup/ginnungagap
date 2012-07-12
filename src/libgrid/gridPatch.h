// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPATCH_H
#define GRIDPATCH_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridPatch.h
 * @ingroup libgridPatch
 * @brief  This file provides the interface to the grid patch.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "../libdata/dataVar.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The patch handle. */
typedef struct gridPatch_struct *gridPatch_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 * @{
 */

/**
 * @brief  Creates a new grid patch.
 *
 * @param[in]  idxLo
 *                The lower left corner of the patch.
 * @param[in]  idxHi
 *                The upper right corner of the patch.
 *
 * @return  Returns a new patch with the requested size.
 */
extern gridPatch_t
gridPatch_new(gridPointUint32_t idxLo, gridPointUint32_t idxHi);


/**
 * @brief  Deletes a grid patch and frees all associated memory.
 *
 * @param[in,out]  *gridPatch
 *                    A pointer to the external variable holding the patch.
 *                    This will be set to @c NULL after completion of the
 *                    function.
 *
 * @return  Returns nothing.
 */
extern void
gridPatch_del(gridPatch_t *gridPatch);


/** @} */

/**
 * @name Getting
 * @{
 */

/**
 * @brief  Retrieves the (logical) size of the patch in a given dimension.
 *
 * @param[in]  patch
 *                The patch that should be queried.
 * @param[in]  idxOfDim
 *                The index of the dimension that should be queried.
 *
 * @return  Returns the extent of the patch in the requested dimension.
 */
extern uint32_t
gridPatch_getOneDim(const gridPatch_t patch, int idxOfDim);


/**
 * @brief  Retrieves the (actual) size of the patch in a given dimension.
 *
 * Depending on the type of the variable, the physical size of the allocated
 * block of data may be differeny from the logical size of the patch, i.e.
 * if the data has to be padded. This function allows to retrieve the size
 * of the allocated data block as opposed to gridPatch_getOneDim() which
 * retrieves the logical size of the patch.
 *
 * @param[in]  patch
 *                The patch that should be queried.
 * @param[in]  idxOfVar
 *                The variable for which to request the size.
 * @param[in]  dim
 *                The index of the dimension that should be queried.
 *
 * @return  Returns the extent of the patch in the requested dimension.
 */
extern uint32_t
gridPatch_getDimActual1D(const gridPatch_t patch, int idxOfVar, int dim);


/**
 * @brief  Retrieves the logical size of the patch.
 *
 * @param[in]   patch
 *                 The patch that should be queried.
 * @param[out]  dim
 *                 This will receive the logical size of the patch.
 *
 * @return  Returns nothing.
 */
extern void
gridPatch_getDims(const gridPatch_t patch, gridPointUint32_t dim);


/**
 * @brief  Retrieves the actual size of the patch for a variable.
 *
 * @param[in]   patch
 *                 The patch that should be queried.
 * @param[in]   idxOfVar
 *                 The index of the variable for which to retrieve the size
 *                 of the patch.
 * @param[out]  dimsActual
 *                 This will receive the actual size of the patch.
 *
 * @return  Returns nothing.
 */
extern void
gridPatch_getDimsActual(const gridPatch_t patch,
                        int               idxOfVar,
                        gridPointUint32_t dimsActual);


/**
 * @brief  Retrieves the (logical) number of cell contained in a patch.
 *
 * @param[in]  patch
 *                The patch to query.
 *
 * @return  Returns the number of cells.
 */
extern uint64_t
gridPatch_getNumCells(const gridPatch_t patch);


/**
 * @brief  Retrieves the (actual) number of cells contained in a patch for a
 *         given variable.
 *
 * @param[in]  patch
 *                The patch to query.
 * @param[in]  idxOfVar
 *                The index of the variable for which to retrieve the actual
 *                number of cells.
 *
 * @return  Returns the actual number of cells.
 */
extern uint64_t
gridPatch_getNumCellsActual(const gridPatch_t patch, int idxOfVar);


/**
 * @brief  Retrieves the lower left corner of the patch.
 *
 * @param[in]   patch
 *                 The patch to query.
 * @param[out]  idxLo
 *                 This will receive the lower left corner of the grid.
 *
 * @return  Returns nothing.
 */
extern void
gridPatch_getIdxLo(const gridPatch_t patch, gridPointUint32_t idxLo);


/** @} */

/**
 * @name  Dealing with Variables
 * @{
 */

/**
 * @brief  Attaches a variable to the patch.
 *
 * @param[in,out]  patch
 *                    The patch the variable should be attached to.
 * @param[in,out]  var
 *                    The variable that should be attached.  The function
 *                    will create a new reference, hence the calling
 *                    function is still responsible for it's reference to
 *                    the variable.
 *
 * @return  Returns the position at which the variable has been added.
 */
extern int
gridPatch_attachVar(gridPatch_t patch, dataVar_t var);


/**
 * @brief  Detaches a variable from a patch.
 *
 * This will also free allocated data for the variable.
 *
 * @param[in,out]  patch
 *                    The patch from which the variable should be detached.
 * @param[in]      idxOfVar
 *                    The index of the variable that should be detached.
 *
 * @return  Returns the variable.
 */
extern dataVar_t
gridPatch_detachVar(gridPatch_t patch, int idxOfVar);


/**
 * @brief  Allocates memory for a variable.
 *
 * This function will do nothing if memory for the requested variable is
 * already allocated.
 *
 * @param[in,out]   patch
 *                     The patch to work on.
 * @param[in]       idxOfVarData
 *                     The index of the variable for which memory should be
 *                     allocated.
 *
 * @return  Returns a pointer to the newly allocated memory region.
 */
extern void *
gridPatch_allocateVarData(gridPatch_t patch, int idxOfVarData);


/**
 * @brief  Frees the data memory associated with a variable.
 *
 * @param[in,out]  patch
 *                    The patch to work on.
 * @param[in]      idxOfVarData
 *                    The index of the variable data to free.
 *
 * @return  Returns nothing.
 */
extern void
gridPatch_freeVarData(gridPatch_t patch, int idxOfVarData);


/**
 * @brief  Replaces the data memory associated with a variable with a new
 *         memory block.
 *
 * If memory had been allocated previously for the variable, it will be
 * freed.
 *
 * @param[in,out]  patch
 *                    The patch to work with.
 * @param[in]      idxOfVarData
 *                    The index of the data memory to be replaced.
 * @param[in]      *newData
 *                    Pointer to the new memory block, it is the
 *                    responsibility of the calling function to ensure that
 *                    the memory has the expected size for this patch and
 *                    the variable.  This may be @c NULL, in which case the
 *                    function behaves as gridPatch_freeVarData().
 *
 * @return  Returns nothing.
 */
extern void
gridPatch_replaceVarData(gridPatch_t patch, int idxOfVarData, void *newData);

/**
 * @brief  Retrieves (and removes) the data of for the requested variable.
 *
 * @param[in,out]  patch
 *                    The patch to work with.
 * @param[in]      idxOfVarData
 *                    The index of the variable for which to pop the data.
 *
 * @return  Returns the pointer to the data.  This might be @c NULL if the
 *          data for the requested variable was not allocated.
 */
extern void *
gridPatch_popVarData(gridPatch_t patch, int idxOfVarData);


/**
 * @brief  Returns a handle to a variable attached to the patch.
 *
 * @param[in]  patch
 *                The patch to query.
 * @param[in]  idxOfVar
 *                The index of the variable.
 *
 * @return  Returns the internal handle of the patch to the variable.
 */
extern dataVar_t
gridPatch_getVarHandle(const gridPatch_t patch, int idxOfVar);


/**
 * @brief  Retrieves a pointer to the allocated data.
 *
 * If no memory has been allocated for the variable yet, is will be done
 * now.
 *
 * @param[in]  patch
 *                The patch to work with.
 * @param[in]  idxOfVarData
 *                The index of the data to retrieve.
 *
 * @return  Returns a pointer to the memory region.  If no memory was
 *          allocated before, the function will call
 *          gridPatch_allocateVarData() and return the result of this
 *          function, hence this function always returns a pointer to a
 *          valid memory region holding the data.
 */
extern void *
gridPatch_getVarDataHandle(const gridPatch_t patch, int idxOfVarData);


/**
 * @brief  Retrieves a pointer to allocated data by searching for a provided
 *         variable.
 *
 * This performs a linear search on the attached variables and then uses the
 * found index in a call to gridPatch_getVarDataHandle() to retrieve the
 * data handle.
 *
 * @param[in]  patch
 *                The patch to work with.
 * @param[in]  var
 *                The variable to look for.
 *
 * @return  Returns a pointer to the memory, or @c NULL if the variable has
 *          not been found.
 */
extern void *
gridPatch_getVarDataHandleByVar(const gridPatch_t patch,
                                const dataVar_t   var);


/**
 * @brief  Queries for the number of variables attached to the patch.
 *
 * @param[in]  patch
 *                The patch to query.
 *
 * @return  Returns the number of variables attached to the patch.
 */
extern int
gridPatch_getNumVars(const gridPatch_t patch);


/** @} */

/**
 * @name  Using
 * @{
 */

/**
 * @brief  This will transpose a given patch and all allocated data arrays.
 *
 * @param[in]  patch
 *                The patch to transpose.
 * @param[in]  dimA
 *                The first dimension to exchange.  This must be between
 *                (inclusive) 0 and (exclusive) #NDIM.
 * @param[in]  dimB
 *                The second dimension to exchange with.  This must be
 *                between (inclusive) 0 and (exclusive) #NDIM.
 *
 * @return  Returns nothing.
 */
extern void
gridPatch_transpose(gridPatch_t patch,
                    int         dimA,
                    int         dimB);


/**
 * @brief  Performs a copy of the variable data in a subset of the patch.
 *
 * @param[in]   patch
 *                 The patch to work with.
 * @param[in]   idxVar
 *                 The variable for which to copy the data.
 * @param[in]   idxLo
 *                 The lower left corner of the window which should be
 *                 copied.  This must be within the patch.
 * @param[in]   idxHi
 *                 The upper right corner of the window which should be
 *                 copied.  This must be within the patch.
 * @param[out]  *numElements
 *                 Pointer to an external variable that will receive the
 *                 number of elemets that have been copied.  This may be
 *                 @c NULL.
 *
 * @return  Returns a pointer to a new memory blob that holds the requested
 *          variable data in the specified window.
 *
 * @bug  This does not work for padded data, i.e. data for which the logical
 *       patch dimension is different from the actual dimension.
 */
extern void *
gridPatch_getWindowedDataCopy(const gridPatch_t patch,
                              int               idxVar,
                              gridPointUint32_t idxLo,
                              gridPointUint32_t idxHi,
                              uint64_t          *numElements);


/**
 * @brief  This will put data into a subset of the patch.
 *
 * @param[in,out]  patch
 *                    The patch to work with.
 * @param[in]      idxVar
 *                    The variable for which to add the data.
 * @param[in]      idxLo
 *                    The lower left corner of the window, must be within
 *                    the patch.
 * @param[in]      idxHi
 *                    The upper right corner of the window, must be within
 *                    the patch.
 * @param[in]      *data
 *                    The data that should be copied in.
 */
extern void
gridPatch_putWindowedData(gridPatch_t       patch,
                          int               idxVar,
                          gridPointUint32_t idxLo,
                          gridPointUint32_t idxHi,
                          const void        *data);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridPatch Patches
 * @ingroup libgrid
 * @brief This provides grid patches.
 */


#endif
