// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERHDF5_ADT_H
#define GRIDREADERHDF5_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5_adt.h
 * @ingroup  libgridIOInHDF5
 * @brief  Implements the main structure of the HDF5 reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_adt.h"
#include <hdf5.h>


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure for the HDF5 reader. */
struct gridReaderHDF5_struct {
	/** @brief  The base structure. */
	struct gridReader_struct base;
	/** @brief  The HDF5 file handle. */
	hid_t file;
};

/*--- Prototypes of protected functions ---------------------------------*/

/**
 * @name  Creating and Deleting (Protected)
 *
 * @{
 */

/**
 * @brief  Allocates memory for an HDF5 grid reader.
 *
 * @return  Returns a handle to a new (uninitialized) HDF5 reader structure.
 */
extern gridReaderHDF5_t
gridReaderHDF5_alloc(void);


/**
 * @brief  Sets all required fields of the HDF5 reader structure to safe
 *         initial values.
 *
 * @param[in,out]  reader
 *                    The reader to initialize.  This must be a valid reader
 *                    object.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderHDF5_init(gridReaderHDF5_t reader);


/**
 * @brief  Frees all members of the Reader writer structure.
 *
 * @param[in,out]  reader
 *                    The reader to work with,  This must be a valid reader,
 *                    passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderHDF5_free(gridReaderHDF5_t reader);

/** @} */

/**
 * @name Setting (Protected)
 *
 * @{
 */

/**
 * @brief  Sets the underlying HDF5 file handle.
 *
 * @param[in,out]  reader
 *                    The reader object for which to set a new HDF5 file
 *                    handle.  If a file handle is already attached, it will
 *                    be closed first.
 * @param[in]      hid
 *                    The new file handle.
 *
 * @return  Returns nothing.
 */
extern void
gridReaderHDF5_setHid(gridReaderHDF5_t reader, hid_t hid);

/** @} */



#endif
