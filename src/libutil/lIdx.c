// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/lIdx.c
 * @ingroup   libutilMiscLIdx
 * @brief  Implements the exported function for the linear indices.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "lIdx.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Implementations of exported functions -----------------------------*/
extern uint64_t
lIdx_fromCoordNd(const uint32_t *restrict coords,
                 const uint32_t *restrict dims,
                 uint32_t                 nd)
{
	uint64_t idx    = 0;
	uint64_t offset = 1;

	for (uint32_t i = 0; i < nd; i++) {
		idx    += coords[i] * offset;
		offset *= dims[i];
	}

	return idx;
}

extern void
lIdx_toCoordNd(uint64_t                 idx,
               const uint32_t *restrict dims,
               uint32_t                 nd,
               uint32_t *restrict       coords)
{
	int i;

	for (i = 0; i < nd - 1; i++) {
		coords[i] = idx % dims[i];
		idx       = (idx - coords[i]) / dims[i];
	}
	coords[i] = idx;
}
