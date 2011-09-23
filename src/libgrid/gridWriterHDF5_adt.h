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
#include <hdf5.h>


/*--- ADT implementation ------------------------------------------------*/
struct gridWriterHDF5_struct {
	GRIDWRITER_T_CONTENT
	/** @brief  The HDF5 file handle. */
	hid_t file;
};


#endif
