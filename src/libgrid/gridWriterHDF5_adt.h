// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERHDF5_ADT_H
#define GRIDWRITERHDF5_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterHDF5_adt.h
 * @ingroup  libgridIOOutHDF5
 * @brief  Implements the main structure for the HDF5 writer object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter_adt.h"
#include <stdbool.h>
#include <hdf5.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure. */
struct gridWriterHDF5_struct {
	/** @brief  The base structure. */
	struct gridWriter_struct base;
	/** @brief  The HDF5 file handle. */
	hid_t                    fileHandle;
#ifdef WITH_MPI
	/** @brief  The MPI communicator to be used. */
	MPI_Comm mpiComm;
#endif
	/** @brief  Toggles the writing of chunked data. */
	bool         doChunking;
	/** @brief  Gives the chunk size. */
	hsize_t      chunkSize[NDIM];
	/** @brief  Toggles the checksum calcluation (requires chunking). */
	bool         doChecksum;
	/** @brief  Toggles the compression (requires chunking). */
	bool         doCompression;
	/** @brief  Selects the compression filter. */
	H5Z_filter_t compressionFilter;
	/** @brief	Toggles region to write patch. */
	bool		 doPatch;
	/** @brief	Gives region to write idxLo. */
	gridPointUint32_t	 rtwLo;
	/** @brief	Gives region to write dims. */
	gridPointUint32_t	 rtwDims;
	 
};


/*--- Prototypes of protected functions ---------------------------------*/

/**
 * @name  Creating and Deleting (Protected, Final)
 *
 * Those are the functions that are only available from within the basic
 * writer and the ones that inherit from the basic writer (the OO equivalent
 * would be @a protected).
 *
 * @{
 */

/**
 * @brief  Allocates memory for an HDF5 grid writer.
 *
 * @return  Returns a handle to a new (uninitialized) HDF5 writer structure.
 */
extern gridWriterHDF5_t
gridWriterHDF5_alloc(void);


/**
 * @brief  Sets all required fields of the HDF5 writer structure to safe
 *         initial values.
 *
 * @param[in,out]  writer
 *                    The writer to initialize.  This must be a valid writer
 *                    object.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterHDF5_init(gridWriterHDF5_t writer);


/**
 * @brief  Frees all members of the HDF5 writer structure.
 *
 * @param[in,out]  writer
 *                    The writer to work with,  This must be a valid writer,
 *                    passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
gridWriterHDF5_free(gridWriterHDF5_t writer);

#endif
