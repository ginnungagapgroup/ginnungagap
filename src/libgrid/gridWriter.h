// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
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
#include "../libutil/filename.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a grid writer.
 */
typedef struct gridWriter_struct *gridWriter_t;


/*--- Prototypes of virtual functions -----------------------------------*/

/**
 * @name  Creating and Deleting (Public, Virtual, Abstract)
 *
 * Note that the base writer cannot be instantiated.
 *
 * @{
 */

/**
 * @brief  Deletes a writer object and frees the associated memory.
 *
 * @param[in,out]  *writer
 *                    A pointer to the external variable holding the
 *                    refernce to the writer object that should be
 *                    deleted.  The external variable will be set to
 *                    NULL after the writer has been deleted.
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_del(gridWriter_t *writer);


/** @} */

/**
 * @name  Using (Public, Virtual, Abstract)
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
 * @name  Additional Initialization (Virtual, Abstract)
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

/*--- Prototypes of final functions -------------------------------------*/

/**
 * @name  Getting/Setting (Public, Final)
 *
 * @{
 */

/**
 * @brief  Sets whether the writer should overwrite an existing file (only
 *         on first open)
 *
 * @param[in,out]  writer
 *                    The writer to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      overwriteFileIfExists
 *                    If this is @c true, then on the very first activation
 *                    of the reader the target file will be overwritten, if
 *                    it already exists.  If @c false is passed, the writing
 *                    will fail with an error if the file already exists.
 *                    If the target file does not exists on the first
 *                    activation, this flag has no effect.
 *
 * @return  Returns nothings.
 */
extern void
gridWriter_setOverwriteFileIfExists(gridWriter_t writer,
                                    bool         overwriteFileIfExists);


/**
 * @brief  Retrieves the flag dealing with overwriting the target file.
 *
 * @param[in]  writer
 *                The writer object to query.  Passing @c NULL is undefined.
 *
 * @return  Returns @c true if the target file would be overwritten on the
 *          first activation of the writer.  If the file would not be
 *          overwritten, @c false is returned.
 */
extern bool
gridWriter_getOverwriteFileIfExists(const gridWriter_t writer);


/**
 * @brief  Sets a new file name for the writer.
 *
 * This function may only be called when the writer is deactivated.
 *
 * @param[in,out]  writer
 *                    The writer to work with.  Passing @c NULL is
 *                    undefined as is passing a writer that is currently
 *                    activated.
 * @param[in]      fileName
 *                    The new filename object to use.  Passing @c NULL is
 *                    undefined.  The caller relinquishes control over to
 *                    object.
 *
 * @return  Returns nothing.
 */
extern void
gridWriter_setFileName(gridWriter_t writer,
                       filename_t   fileName);

/**
 * @brief  Will update the filename of the writer with the (set) fields in
 *         the file name object.
 *
 * This function may only be called when the writer is deactivated.
 *
 * @param[in,out]  writer
 *                    The writer to work with.  Passing @c NULL is
 *                    undefined as is passing a writer that is currently
 *                    activated.
 * @param[in]      fileName
 *                    The filename object from which to take the updated
 *                    information.  The caller has to free the filename
 *                    object.
 *
 * @return  Returns nothing.                    
 */
extern void
gridWriter_overlayFileName(gridWriter_t writer,
                           const filename_t fileName);


/**
 * @brief  Retrieves the current filename the writer uses.
 *
 * @param[in]  writer
 *                The writer to query, this must be a valid writer, passing
 *                @c NULL is undefined.
 *
 * @return  Returns the internal filename object, the caller may only use it
 *          read-only.
 */
extern const filename_t
gridWriter_getFileName(const gridWriter_t writer);


/** @} */


/**
 * @name  Using (Public, Final)
 *
 * @{
 */

/**
 * @brief  Queries whether the reader is active or not.
 *
 * @param[in]  writer
 *                The writer to query, must be a valid writer object.
 *                Passing @c NULL is undefined.
 *
 * @return  Returns @c true if the writer is active and @c false if not.
 */
extern bool
gridWriter_isActive(const gridWriter_t writer);

/**
 * @brief  Checks if the writer has been activated already.
 *
 * @param[in]  writer
 *                The writer to query, must be a valid writer, passing
 *                @c NULL is undefined.
 *
 * @return  Returns @c false if the writer has never been activated with the
 *          current file name.  If the writer already has been activated
 *          before (and possibly also been deactivated) @c true is returned.
 */
extern bool
gridWriter_hasBeenActivated(const gridWriter_t writer);

/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOOut Writing
 * @ingroup libgridIO
 * @brief This provides functionality to write grids.
 */


#endif
