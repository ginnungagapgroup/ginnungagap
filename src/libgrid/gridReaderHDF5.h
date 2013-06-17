// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERHDF5_H
#define GRIDREADERHDF5_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5.h
 * @ingroup  libgridIOInHDF5
 * @brief  Provides the interface for the HDF5 reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "../libutil/parse_ini.h"
#include <hdf5.h>


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Handle for the HDF5 reader. */
typedef struct gridReaderHDF5_struct *gridReaderHDF5_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */

/** @copydoc gridReader_del() */
extern void
gridReaderHDF5_del(gridReader_t *reader);

/** @} */

/**
 * @name  Using (Virtual)
 *
 * @{
 */

/** @copydoc gridReader_readIntoPatch() */
extern void
gridReaderHDF5_readIntoPatch(gridReader_t reader, gridPatch_t patch);

/** @copydoc gridReader_readIntoPatchForVar() */
extern void
gridReaderHDF5_readIntoPatchForVar(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar);


/** @} */


/*--- Prototypes of final functions -------------------------------------*/

/**
 * @name  Creating and Deleting (Final)
 *
 * @{
 */

/**
 * @brief  Creates a new empty HDF5 reader.
 *
 * @return  The new writer.
 */
extern gridReaderHDF5_t
gridReaderHDF5_new(void);

/** @} */

/**
 * @name  Getting (Final)
 *
 * @{
 */

/**
 * @brief  Retrieves a the underlying HDF5 file handle from a HDF5 grid
 *         reader.
 *
 * @param[in]  reader
 *                The reader that should be queried, passing @c NULL is
 *                undefined.
 *
 * @return  Returns a handle to the internal HDF5 file handle, the caller
 *          must not try free the object.
 */
extern hid_t
gridReaderHDF5_getH5File(const gridReaderHDF5_t writer);

/** @} */

/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInHDF5 HDF5 Reader
 * @ingroup libgridIOIn
 * @brief  Provides the HDF5 reader.
 *
 * Note that the HDF5 reader will open the underlying HDF5 file on setting
 * or resetting the file name (via gridReader_setFileName() or
 * gridReader_overlayFileName()). The file will stay opened until the reader
 * is destroyed, or another file handle is set via
 * gridReaderHDF5_setH5File().
 */

#endif
