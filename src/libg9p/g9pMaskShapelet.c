// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMaskShapelet.c
 * @ingroup  libg9pMaskShapelet
 * @brief  Implements the mask shapelet.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMaskShapelet.h"
#include <assert.h>
#include <math.h>
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pMaskShapelet_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static g9pMaskShapelet_t
local_new(int8_t lowLevel, int8_t highLevel);


/*--- Implementations of exported functions -----------------------------*/
extern g9pMaskShapelet_t
g9pMaskShapelet_new(int8_t lowLevel, int8_t highLevel)
{
	assert(lowLevel >= 0);
	assert(lowLevel <= highLevel);

	int8_t            lev[NDIM];
	g9pMaskShapelet_t shapelet = local_new(lowLevel, highLevel);
	const int8_t      extent   = shapelet->extent;
	const int8_t      dim      = shapelet->dim1D;
	size_t            idx      = 0;
#if (NDIM == 3)
	for (int8_t k = 0; k < dim; k++)
#endif
	{
#if (NDIM == 3)
		lev[2] = k <= extent ? k : dim - 1 - k;
#endif
		for (int8_t j = 0; j < dim; j++) {
			lev[1] = j <= extent ? j : dim - 1 - j;
#if (NDIM == 3)
			lev[1] = (lev[1] > lev[2]) ? lev[2] : lev[1];
#endif
			for (int8_t i = 0; i < dim; i++) {
				lev[0]                = i <= extent ? i : dim - 1 - i;
				shapelet->data[idx++] = (lev[0] > lev[1] ? lev[1] : lev[0])
				                        + lowLevel;
			}
		}
	}

	return shapelet;
}

extern void
g9pMaskShapelet_del(g9pMaskShapelet_t *shapelet)
{
	assert(shapelet != NULL && *shapelet != NULL);

	xfree((*shapelet)->data);
	xfree(*shapelet);
	*shapelet = NULL;
}

extern int8_t
g9pMaskShapelet_getLowLevel(const g9pMaskShapelet_t sl)
{
	assert(sl != NULL);

	return sl->low;
}

extern int8_t
g9pMaskShapelet_getHighLevel(const g9pMaskShapelet_t sl)
{
	assert(sl != NULL);

	return sl->high;
}

extern int8_t
g9pMaskShapelet_getExtent(const g9pMaskShapelet_t sl)
{
	assert(sl != NULL);

	return sl->extent;
}

extern int8_t
g9pMaskShapelet_getDim1D(const g9pMaskShapelet_t sl)
{
	assert(sl != NULL);

	return sl->dim1D;
}

extern const int8_t *
g9pMaskShapelet_getData(const g9pMaskShapelet_t sl)
{
	assert(sl != NULL);

	return sl->data;
}

/*--- Implementations of local functions --------------------------------*/
static g9pMaskShapelet_t
local_new(int8_t lowLevel, int8_t highLevel)
{
	g9pMaskShapelet_t shapelet;

	shapelet         = xmalloc(sizeof(struct g9pMaskShapelet_struct));
	shapelet->low    = lowLevel;
	shapelet->high   = highLevel;
	shapelet->extent = highLevel - lowLevel;
	assert(shapelet->extent < 63);
	shapelet->dim1D  = 2 * shapelet->extent + 1;

	assert(shapelet->dim1D % 2 == 1);
	assert((shapelet->dim1D == (shapelet->extent * 2 + 1))
	       || ((shapelet->extent == 0) && (shapelet->dim1D == 1)));

	shapelet->data = xmalloc(sizeof(int8_t) * POW_NDIM(shapelet->dim1D));

	return shapelet;
}
