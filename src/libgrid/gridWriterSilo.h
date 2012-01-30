// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERSILO_H
#define GRIDWRITERSILO_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterSilo.h
 * @ingroup  libgridIOOutSilo
 * @brief  Provides the interface for the Silo writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include "gridRegular.h"
#include "gridPatch.h"
#include "gridPoint.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the Silo writer object. */
typedef struct gridWriterSilo_struct *gridWriterSilo_t;


/*--- Prototypes of implemented abstract functions ----------------------*/

/**
 * @name  Creating and Deleting (Public, Virtual)
 *
 * @{
 */

/** @copydoc gridWriter_del() */
extern void
gridWriterSilo_del(gridWriter_t *writer);


/** @} */


/**
 * @name  Using (Public, Virtual)
 *
 * @{
 */

/** @copydoc gridWriter_activate() */
extern void
gridWriterSilo_activate(gridWriter_t writer);


/** @copydoc gridWriter_deactivate() */
extern void
gridWriterSilo_deactivate(gridWriter_t writer);


/** @copydoc gridWriter_writeGridPatch() */
extern void
gridWriterSilo_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta);


/** @copydoc gridWriter_writeGridRegular() */
extern void
gridWriterSilo_writeGridRegular(gridWriter_t  writer,
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
gridWriterSilo_initParallel(gridWriter_t writer, MPI_Comm mpiComm);


/** @} */

#endif

/*--- Prototypes of final functions -------------------------------------*/

/**
 * @name  Creating and Deleting (Final)
 *
 * @{
 */

/**
 * @brief  Creates a new empty Silo writer.
 *
 * @return  The new writer.
 */
extern gridWriterSilo_t
gridWriterSilo_new(void);


/** @} */

/**
 * @name  Setting (Public, Final)
 *
 * @{
 */

/**
 * @brief  Sets the underlying file backend type of the Silo file.
 *
 * @param[in,out]  w
 *                    The writer for which to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      dbType
 *                    The database type.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterSilo_setDbType(gridWriterSilo_t w, int dbType);


/**
 * @brief  Sets the number of files the data will be spread over.
 *
 * @param[in,out]  w
 *                    The writer to work with.  Must be a valid writer,
 *                    passinge @c NULL is undefined.
 * @param[in]      numFiles
 *                    The new number of files.  Must be at least 1.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterSilo_setNumFiles(gridWriterSilo_t w, int numFiles);


/** @} */

/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOOutSilo Silo Writer
 * @ingroup libgridIOOut
 * @brief  Provides the Silo Writer.
 *
 * @section libgridIOOutSiloIniFormat  Expected Format for Ini Files
 *
 * @code
 * [SectionName]
 * @endcode
 */


#endif
