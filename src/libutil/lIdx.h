// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef LIDX_H
#define LIDX_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/lIdx.h
 * @ingroup  libutilMisc
 * @brief  Provides the interface to linear indices (lIdx).
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Translates coordinates to linear index.
 *
 * @param[in]  coords
 *                The coordinates.
 * @param[in]  dims
 *                The extensions.
 *
 * @return  Returns the linear index.
 */
static inline uint64_t
lIdx_fromCoord2d(const uint32_t coords[2], const uint32_t dims[2])
{
	return coords[0] + coords[1] * dims[0];
}

static inline void
lIdx_toCoord2d(uint64_t idx, const uint32_t dims[3], uint32_t coords[3])
{
	coords[0] = idx % dims[0];
	idx       = (idx - coords[0]) / dims[0];
	coords[1] = idx;
}

static inline uint64_t
lIdx_fromCoord3d(const uint32_t coords[3], const uint32_t dims[3])
{
	return coords[0] + (coords[1] + coords[2] * dims[1]) * dims[0];
}

static inline void
lIdx_toCoord3d(uint64_t idx, const uint32_t dims[3], uint32_t coords[3])
{
	coords[0] = idx % dims[0];
	idx       = (idx - coords[0]) / dims[0];
	coords[1] = idx % dims[1];
	idx       = (idx - coords[1]) / dims[1];
	coords[2] = idx;
}

#if (NDIM == 2)
#  define lIdx_fromCoordNdim(a, b)  lIdx_fromCoord2d(a, b)
#  define lIdx_toCoordNdim(a, b, c) lIdx_toCoord2d(a, b, c)
#elif (NDIM == 3)
#  define lIdx_fromCoordNdim(a, b)  lIdx_fromCoord3d(a, b)
#  define lIdx_toCoordNdim(a, b, c) lIdx_toCoord3d(a, b, c)
#else
#  define lIdx_fromCoordNdim(a, b)  lIdx_fromCoordNd(a, b, NDIM)
#  define lIdx_toCoordNdim(a, b, c) lIdx_toCoordNd(a, b, NDIM, c)
#endif

extern uint64_t
lIdx_fromCoordNd(const uint32_t *restrict coord,
                 const uint32_t *restrict dims,
                 uint32_t                 nd);

extern void
lIdx_toCoordNd(uint64_t                 idx,
               const uint32_t *restrict dims,
               uint32_t                 nd,
               uint32_t *restrict       coords);

#endif
