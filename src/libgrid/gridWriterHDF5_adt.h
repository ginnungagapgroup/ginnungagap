// Copyright (C) 2011, Steffen Knollmann
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
struct gridWriterHDF5_struct {
	GRIDWRITER_T_CONTENT
	/** @brief  The HDF5 file handle. */
	hid_t      fileHandle;
	/** @brief  Gives the name of the HDF5 file. */
	const char *fileName;
#ifdef WITH_MPI
	/** @brief  The MPI communicator to be used. */
	MPI_Comm mpiComm;
#endif
	/** @brief  Toggles whether an existing file should be overwritten. */
	bool         force;
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
};


#endif
