// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDUTILHDF5_H
#define GRIDUTILHDF5_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file gridUtilHDF5.h
 * @ingroup  GROUP
 * @brief  SHORT DESC
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <hdf5.h>
#include "../libutil/diediedie.h"


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Selects an hyperslab for a given offset and extent.
 *
 * @param[in,out]  ds
 *                    The dataspace on which to select the hyperslab.
 * @param[in]      idxLo
 *                    The offset of the selection.
 * @param[in]      dims
 *                    The extent of the selection.
 *
 * @return  Returns nothing.
 */
inline static void
gridUtilHDF5_selectHyperslab(hid_t             ds,
                             gridPointUint32_t idxLo,
                             gridPointUint32_t dims)
{
	hsize_t dsDims[NDIM];
	hsize_t dsIdxLo[NDIM];

	for (int i = 0; i < NDIM; i++) {
		dsDims[i]  = dims[NDIM - 1 - i];
		dsIdxLo[i] = (idxLo == NULL) ? 0 : idxLo[NDIM - 1 - i];
	}

	H5Sselect_hyperslab(ds, H5S_SELECT_SET, dsIdxLo, NULL, dsDims, NULL);
}

/**
 * @brief  Creates an HDF5 data space for a given size.
 *
 * @param[in]  dims
 *                The size of the data space.
 *
 * @return  Returns a proper HDF5 data space for the given size.
 */
inline static hid_t
gridUtilHDF5_getDataSpaceFromDims(gridPointUint32_t dims)
{
	hid_t   ds;
	hsize_t dsDims[NDIM];

	for (int i = 0; i < NDIM; i++)
		dsDims[i] = dims[NDIM - 1 - i];

	ds = H5Screate_simple(NDIM, dsDims, NULL);
	if (ds < 0)
		diediedie(EXIT_FAILURE);

	return ds;
}

#endif
