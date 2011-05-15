// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITER_H
#define GRIDWRITER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriter.h
 * @ingroup libgridIOOut
 * @brief  This file provides the interface to an abstract grid writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a grid writer.
 */
typedef struct gridWriter_struct *gridWriter_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new writer object from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file in which to look for the construction
 *                    information for this writer.
 * @param[in]      *sectionName
 *                    The section name within the ini file that holds
 *                    the construction information.
 *
 * @return  Returns a new writer object that is constructed from the
 *          information in the ini file.
 */
extern gridWriter_t
gridWriter_newFromIni(parse_ini_t ini, const char *sectionName);


/**
 * @brief  Deletes a writer object and frees the associated memory.
 *
 * @param[in,out]  *writer
 *                    A pointer to the external variable holding the
 *                    refernce to the writer object that should be
 *                    deleted.  The external variable will be set to
 *                    NULL after the writer has been deleted.
 *
 * @param  Returns nothing.
 */
extern void
gridWriter_del(gridWriter_t *writer);


/** @} */

/**
 * @name  Using
 *
 * @{
 */

/**
 * @brief  This will activate the writer for actual IO.
 *
 * This function must be called before any IO can be performed.  In
 * essences, it will make sure that the output file or files are opened
 * and available.
 *
 * @param[in,out]  writer
 *                    The writer to activate.
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_activate(gridWriter_t writer);


/**
 * @brief  This will yield the IO ressources used by the writer.
 *
 * This functions must be called after the IO has been done.
 * Effectively, the function will properly close the file or files and
 * yield the IO system to other tasks (if any).
 *
 * @param[in,out]  writer
 *                    The writer to deactivate.
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_deactivate(gridWriter_t writer);


/**
 * @brief  This will write a patch to the file.
 *
 * @param[in,out]  writer
 *                    The writer to use.
 * @param[in,out]  patch
 *                    The patch that should be written.
 * @param[in]      *patchName
 *                    The name used for the patch in the file.
 * @param[in]      origin
 *                    The physical coordinates of the origin of this
 *                    patch.
 * @param[in]      delta
 *                    The spacing of the patch in physical units.
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_writeGridPatch(gridWriter_t   writer,
                          gridPatch_t    patch,
                          const char     *patchName,
                          gridPointDbl_t origin,
                          gridPointDbl_t delta);


/**
 * @brief  This will write a regular grid to the file.
 *
 * @param[in,out]  writer
 * @param[in,out]  grid
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_writeGridRegular(gridWriter_t  writer,
                            gridRegular_t grid);


/** @} */

#ifdef WITH_MPI

/**
 * @name  Additional Initialization
 *
 * @{
 */

/**
 * @brief  This initializes the parallel writing interface for this
 *         reader.
 *
 * @param[in,out]  writer
 *                    The writer objects that should be initialized.
 * @param[in]      mpiComm
 *                    The communicator used to synchronize the file
 *                    access over.
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_initParallel(gridWriter_t writer, MPI_Comm mpiComm);


/** @} */
#endif


#endif
