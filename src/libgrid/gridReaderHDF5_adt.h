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
	GRIDREADER_T_CONTENT
	/** @brief  The name of the HDF5 file */
	char *fileName;
	/** @brief */
	/** @brief  The HDF5 file handle. */
	hid_t file;
};


#endif
