// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADER_H
#define GRIDREADER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReader.h
 * @ingroup libgridIOInInterface
 * @brief  This file provides the interface to an abstract grid reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "../libutil/filename.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a reader object.
 */
typedef struct gridReader_struct *gridReader_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting (Public, Virtual, Abstract)
 *
 * @{
 */

/**
 * @brief  Deletes a reader objects and frees the associated memory.
 *
 * @param[in,out]  *reader
 *                    A pointer to the external variable holding the
 *                    refernce to the reader object that should be
 *                    deleted.  The external variable will be set to
 *                    NULL after the reader has been deleted.
 *
 * @return  Returns nothing.
 */
extern void
gridReader_del(gridReader_t *reader);

/** @} */


/**
 * @name  Using (Public, Virtual, Abstract)
 *
 * @{
 */

/**
 * @brief  Reads from the file and fills a provided patch.
 *
 * This will create a new variable, attach it to the patch and fill it
 * with the data from the file.
 *
 * @param[in,out]  reader
 *                    The reader that should be used.
 * @param[in,out]  patch
 *                    The patch which should be filled.
 *
 * @return  Returns nothing.
 */
extern void
gridReader_readIntoPatch(gridReader_t reader, gridPatch_t patch);


/**
 * @brief  Reads from the file and fills only a specific variable of the
 *         patch.
 *
 * This works like gridReader_readIntoPatch() but an already existing
 * variable is filled.
 *
 * @param[in,out]  reader
 *                    The reader that should be used.
 * @param[in,out]  patch
 *                    The patch which should be filled.
 * @param[in]      idxOfVar
 *                    The index of the variable in the patch that should
 *                    be used.
 *
 * @return  Returns nothing.
 */
extern void
gridReader_readIntoPatchForVar(gridReader_t reader,
                               gridPatch_t  patch,
                               int          idxOfVar);

/** @} */


/*--- Prototypes of final functions -------------------------------------*/

/**
 * @name  Setter (Public, Final)
 *
 * @{
 */

/**
 * @brief  Sets a new file name for the reader.
 *
 * @param[in,out]  reader
 *                    The reader to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      fileName
 *                    The new filename object to use.  Passing @c NULL is
 *                    undefined.  The caller relinquishes control over to
 *                    object.
 *
 * @return  Returns nothing.
 */
extern void
gridReader_setFileName(gridReader_t reader,
                       filename_t   fileName);

/**
 * @brief  Will update the filename of the reader with the (set) fields in
 *         the file name object.
 *
 * @param[in,out]  reader
 *                    The reader to work with.  Passing @c NULL is
 *                    undefined.
 * @param[in]      fileName
 *                    The filename object from which to take the updated
 *                    information.  The caller has to free the filename
 *                    object.
 *
 * @return  Returns nothing.                    
 */
extern void
gridReader_overlayFileName(gridReader_t reader,
                           const filename_t fileName);

/** @} */

/**
 * @name  Getter (Public, Final)
 *
 * @{
 */


/**
 * @brief  Retrieves the current filename the reader uses.
 *
 * @param[in]  reader
 *                The reader to query, this must be a valid reader, passing
 *                @c NULL is undefined.
 *
 * @return  Returns the internal filename object, the caller may only use it
 *          read-only.
 */
extern const filename_t
gridReader_getFileName(const gridReader_t reader);

/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOIn Reading
 * @ingroup libgridIO
 * @brief This provides functionality to read grids.
 */

/**
 * @defgroup libgridIOInInterface Interface
 * @ingroup libgridIOIn
 * @brief This provides the common interface for all reader types.
 */


#endif
