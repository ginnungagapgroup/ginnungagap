// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPOINT_H
#define GRIDPOINT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridPoint.h
 * @ingroup libgridPoint
 * @brief  Provides grid points.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdint.h>


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  A #NDIM tuple of integers. */
typedef int      gridPointInt_t[NDIM];

/** @brief  A #NDIM tuple of 32bit unsigned integers. */
typedef uint32_t gridPointUint32_t[NDIM];

/** @brief  A #NDIM tuple of 64bit signed integers. */
typedef int64_t  gridPointInt64_t[NDIM];

/** @brief  A #NDIM tuple of doubles. */
typedef double   gridPointDbl_t[NDIM];

/** @brief  A #NDIM tuple of #fpv_t. */
typedef fpv_t    gridPointFpv_t[NDIM];


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridPoint Points
 * @ingroup libgrid
 * @brief Defines various points to be used in the grids.
 */


#endif
