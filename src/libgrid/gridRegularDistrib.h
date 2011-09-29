// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARDISTRIB_H
#define GRIDREGULARDISTRIB_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridRegularDistrib.h
 * @ingroup libgridRegularDistrib
 * @brief  Provides the interface to the regular grid distribution
 *         object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular.h"
#include "gridPatch.h"
#include <stdint.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT handle --------------------------------------------------------*/

/// @brief  The handle for the regular grid distribution object.
typedef struct gridRegularDistrib_struct *gridRegularDistrib_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating an deleting
 *
 * @{
 */

/**
 * @brief  Creates a new regular grid distribution objects.
 *
 * @param[in,out]  grid
 *                    The grid that should be distributed.  A valid
 *                    regular grid must be passed.  The distribution
 *                    object stores a reference to the grid, the calling
 *                    function keeps the responsibility to dispose of
 *                    its reference to the grid.
 * @param[in]      nProcs
 *                    The processor grid used for distribution.  This
 *                    may be @c NULL indicating that the grid is
 *                    distributed on only one CPU, this is identical to
 *                    passing <code>nProc[i] = 0</code>.
 *
 * @return  Returns a new distribution object.
 */
extern gridRegularDistrib_t
gridRegularDistrib_new(gridRegular_t grid, const gridPointInt_t nProcs);

/**
 * @brief  Deletes a distribution object.
 *
 * This will generally only decrease the reference count to this object.
 * Only when the last reference is deleted, the object is destroyed and the
 * memory is freed.
 *
 * @param[in,out]  *distrib
 *                    Pointer to the external variable holding the reference
 *                    to the distribution object that should be deleted.
 *                    The external variable will be set to @c NULL.  Passing
 *                    @c NULL or a @c NULL reference is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridRegularDistrib_del(gridRegularDistrib_t *distrib);

/** @} */

/**
 * @brief  Creates a new reference to a distribution object.
 *
 * @param[in,out]  distrib
 *                    The distribution object that should be referenced.
 *                    Passing @c NULL is undefined.
 *
 * @return  Returns a reference to @c distrib.
 */
extern gridRegularDistrib_t
gridRegularDistrib_getRef(gridRegularDistrib_t distrib);


#ifdef WITH_MPI

/**
 * @brief  This sets up the MPI details for this distribution object.
 *
 * @param[in,out]  distrib
 *                    The distribution object for which to set up the MPI
 *                    details.  Passing @c NULL is undefined.
 * @param[in]      nProcs
 *                    The number of MPI tasks in each dimension.  All values
 *                    in the vector must be either 0 or positive.  Passing
 *                    @c NULL is valid, in which case the already set values
 *                    for @c nProcs (from gridRegularDistrib_new()) are
 *                    used.
 * @param[in]      comm
 *                    The MPI communicator that should be used for the
 *                    distribution.
 */
extern void
gridRegularDistrib_initMPI(gridRegularDistrib_t distrib,
                           gridPointInt_t       nProcs,
                           MPI_Comm             comm);

extern void
gridRegularDistrib_getProcCoords(gridRegularDistrib_t distrib,
                                 gridPointInt_t       procCoords);

extern MPI_Comm
gridRegularDistrib_getGlobalComm(gridRegularDistrib_t distrib);

#endif

extern int
gridRegularDistrib_getLocalRank(gridRegularDistrib_t distrib);

extern gridPatch_t
gridRegularDistrib_getPatchForRank(gridRegularDistrib_t distrib, int rank);

extern void
gridRegularDistrib_getNProcs(gridRegularDistrib_t distrib,
                             gridPointInt_t       nProcs);

extern gridRegular_t
gridRegularDistrib_getGridHandle(const gridRegularDistrib_t distrib);

/**
 * @brief  Calculates the min and max indices of a given processes in
 *         one dimension.
 *
 * @param[in]   nCells
 *                 The number of cells in the investigated dimension.
 * @param[in]   nProcs
 *                 The number of processors used to distribute the
 *                 @c nCells cells over.  This must be a positive number
 *                 less or equal to @c nCells.
 * @param[in]   rank
 *                 The number of the process to find the minimun and
 *                 maximum indices for.  This must be a value between
 *                 (inclusive) @c 0 and (exclusive) @c nProcs.
 * @param[out]  *idxLo
 *                 Pointer to the external storage position that will
 *                 receive the minimum index.  Passing @c NULL is
 *                 undefined.
 * @param[out]  *idxHi
 *                 Like @c IdxLo but for the maximum index.
 *
 * @return  Returns nothing.
 */
extern void
gridRegularDistrib_calcIdxsForRank1D(uint32_t nCells,
                                     int      nProcs,
                                     int      rank,
                                     uint32_t *idxLo,
                                     uint32_t *idxHi);

/**
 * @brief  Performs a transposition of the distributed grid.
 *
 * @param[in]  distrib
 *                The distribution object to work with.  
 * @param[in]  dimA
 *                The dimension to exchange.
 * @param[in]  dimB
 *                The dimension to exchange with.
 *
 * @return  Returns nothing.
 */
extern void
gridRegularDistrib_transpose(gridRegularDistrib_t distrib,
                             int                  dimA,
                             int                  dimB);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridRegularDistribg
 * @ingroup libgridRegular
 * @brief  Provides a module to distribute a logical grid onto an MPI
 *         processor grid.
 *
 * This works also in non-MPI mode, albeit with reduced functionality.
 * The main functionality implemented in this module is to calculate the
 * according grid patch that a given process holds and to deal with grid
 * transposition (which is why the interface also must be used in the
 * non-MPI case).
 */

#endif
