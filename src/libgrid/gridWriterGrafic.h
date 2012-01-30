// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERGRAFIC_H
#define GRIDWRITERGRAFIC_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterGrafic.h
 * @ingroup  libgridIOOutGrafic
 * @brief  Provides the interface for the Grafic writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/grafic.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the Grafic writer object. */
typedef struct gridWriterGrafic_struct *gridWriterGrafic_t;


/*--- Prototypes of implemented abstract functions ----------------------*/


/**
 * @name  Creating and Deleting (Virtual)
 *
 * @{
 */


/** @copydoc gridWriter_del() */
extern void
gridWriterGrafic_del(gridWriter_t *writer);


/** @} */


/**
 * @name  Using (Virtual)
 *
 * @{
 */

/** @copydoc gridWriter_activate() */
extern void
gridWriterGrafic_activate(gridWriter_t writer);


/** @copydoc gridWriter_deactivate() */
extern void
gridWriterGrafic_deactivate(gridWriter_t writer);


/** @copydoc gridWriter_writeGridPatch() */
extern void
gridWriterGrafic_writeGridPatch(gridWriter_t   writer,
                                gridPatch_t    patch,
                                const char     *patchName,
                                gridPointDbl_t origin,
                                gridPointDbl_t delta);


/** @copydoc gridWriter_writeGridRegular() */
extern void
gridWriterGrafic_writeGridRegular(gridWriter_t  writer,
                                  gridRegular_t grid);


/** @} */

#ifdef WITH_MPI

/**
 * @name  Additional Initialization (Virtual)
 *
 * @{
 */

/** @copydoc gridWriter_initParallel() */
extern void
gridWriterGrafic_initParallel(gridWriter_t writer, MPI_Comm mpiComm);


/** @} */

#endif


/*--- Prototypes of final functions -------------------------------------*/

/**
 * @name  Creating and Deleting (Final)
 *
 * @{
 */

/**
 * @brief  Creates a new empty HDF5 writer.
 *
 * @return  The new writer.
 */
extern gridWriterGrafic_t
gridWriterGrafic_new(void);


/** @} */

/**
 * @name  Getting and Setting (Final)
 *
 * @{
 */

/**
 * @brief  Sets the underlying Grafic object to the provided one.
 *
 * @param[in,out]  writer
 *                    The writer to set the Grafic object for.  Must be a
 *                    proper Grafic grid writer object, passing @c NULL is
 *                    undefined.
 * @param[in]      grafic
 *                    The graphic module that should be set.  The caller
 *                    relinquishes control over the object.  Passing @c NULL
 *                    is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterGrafic_setGrafic(gridWriterGrafic_t writer, grafic_t grafic);

/**
 * @brief  Retrieves a the underlying Grafic object from a Grafic grid
 *         writer.
 *
 * @param[in]  writer
 *                The writer that should be queried, passing @c NULL is
 *                undefined.
 *
 * @return  Returns a handle to the internal Grafic object, the caller must
 *          not try free the object.
 */
extern grafic_t
gridWriterGrafic_getGrafic(const gridWriterGrafic_t writer);

/**
 * @brief  Sets the writer to white noise mode.
 *
 * @param[in,out]  writer
 *                    The writer to deal with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      isWhiteNoise
 *                    Toggles whether white noise is written or not.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterGrafic_setIsWhiteNoise(gridWriterGrafic_t writer,
                                 bool               isWhiteNoise);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOOutGrafic Grafic Writer
 * @ingroup libgridIOOut
 * @brief  Provides the Grafic Writer.
 *
 * @section libgridIOOutGraficIniFormat  Expected Format for Ini Files
 *
 * @code
 * [SectionName]
 * @endcode
 */


#endif
