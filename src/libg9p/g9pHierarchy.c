// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchy.c
 * @ingroup  libg9pHierarchy
 * @brief  Provides the implementation of the hierarchy structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchy.h"
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include "../libutil/xmem.h"
#include "../libutil/tile.h"
#include "../libutil/refCounter.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pHierarchy_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/

static g9pHierarchy_t
local_allocate(uint8_t numLevels);

static void
local_fillFactorsFromDims(g9pHierarchy_t h);

static void
local_fillDimsFromFactors(g9pHierarchy_t h);


/*--- Implementations of exported functions -----------------------------*/
extern g9pHierarchy_t
g9pHierarchy_newWithDims(uint8_t numLevels, const uint32_t *const dim1Ds)
{
	g9pHierarchy_t h = local_allocate(numLevels);

	memcpy(h->dim1Ds, dim1Ds, sizeof(uint32_t) * numLevels);
	local_fillFactorsFromDims(h);

	return g9pHierarchy_getRef(h);
}

extern g9pHierarchy_t
g9pHierarchy_newWithSimpleFactor(uint8_t  numLevels,
                                 uint32_t minDim1D,
                                 uint32_t factor)
{
	g9pHierarchy_t h = local_allocate(numLevels);

	h->dim1Ds[0]  = minDim1D;
	h->factors[0] = UINT8_C(1);
	for (int i = 1; i < numLevels; i++)
		h->factors[i] = factor;

	local_fillDimsFromFactors(h);

	return g9pHierarchy_getRef(h);
}

extern g9pHierarchy_t
g9pHierarchy_newWithExplicitFactors(uint8_t               numLevels,
                                    uint32_t              minDim1D,
                                    const uint32_t *const factors)
{
	g9pHierarchy_t h = local_allocate(numLevels);

	h->dim1Ds[0]  = minDim1D;
	h->factors[0] = UINT8_C(1);
	memcpy(h->factors + 1, factors, sizeof(uint32_t) * (numLevels - 1));

	local_fillDimsFromFactors(h);

	return g9pHierarchy_getRef(h);
}

extern g9pHierarchy_t
g9pHierarchy_getRef(g9pHierarchy_t h)
{
	assert(h != NULL);

	refCounter_ref(&(h->refCounter));

	return h;
}

extern void
g9pHierarchy_del(g9pHierarchy_t *h)
{
	assert(h != NULL && *h != NULL);

	if (refCounter_deref(&((*h)->refCounter))) {
		xfree((*h)->dim1Ds);
		xfree((*h)->factors);
		xfree(*h);
	}

	*h = NULL;
}

extern const uint32_t *
g9pHierarchy_getDim1Ds(const g9pHierarchy_t h, uint32_t *dims)
{
	assert(h != NULL);

	if (dims != NULL)
		memcpy(dims, h->dim1Ds, sizeof(uint32_t) * h->numLevels);

	return h->dim1Ds;
}

extern uint32_t
g9pHierarchy_getDim1DAtLevel(const g9pHierarchy_t h, uint8_t level)
{
	assert(h != NULL);
	assert(level < h->numLevels);

	return h->dim1Ds[level];
}

extern const uint32_t *
g9pHierarchy_getFactors(const g9pHierarchy_t h, uint32_t *factors)
{
	assert(h != NULL);

	if (factors != NULL)
		memcpy(factors, h->factors, sizeof(uint32_t) * h->numLevels);

	return h->factors;
}

extern uint32_t
g9pHierarchy_getFactorToNextLevel(const g9pHierarchy_t h, uint8_t thisLevel)
{
	assert(h != NULL);
	assert(thisLevel < h->numLevels - 1);

	return h->factors[thisLevel + 1];
}

extern uint32_t
g9pHierarchy_getFactorFromPrevLevel(const g9pHierarchy_t h,
                                    uint8_t              thisLevel)
{
	assert(h != NULL);
	assert(thisLevel < h->numLevels && thisLevel > 0);

	return h->factors[thisLevel];
}

extern uint32_t
g9pHierarchy_getFactorBetweenLevel(const g9pHierarchy_t h,
                                   uint8_t              l1,
                                   uint8_t              l2)
{
	assert(h != NULL);
	assert(l1 < h->numLevels);
	assert(l2 < h->numLevels);

	if (l1 > l2) {
		uint8_t tmp = l1;
		l1 = l2;
		l2 = tmp;
	}

	uint32_t fac = 1;

	while (l1 < l2) {
		fac *= g9pHierarchy_getFactorToNextLevel(h, l1);
		l1++;
	}

	return fac;
}


/*--- Implementations of local functions --------------------------------*/
static g9pHierarchy_t
local_allocate(uint8_t numLevels)
{
	assert(numLevels > 0);
	assert(numLevels < G9PHIERARCHY_MAX_NUM_LEVELS);

	g9pHierarchy_t h;

	h            = xmalloc(sizeof(struct g9pHierarchy_struct));
	refCounter_init(&(h->refCounter));
	h->numLevels = numLevels;
	h->dim1Ds    = xmalloc(sizeof(uint32_t) * numLevels);
	h->factors   = xmalloc(sizeof(uint32_t) * numLevels);

	return h;
}

static void
local_fillFactorsFromDims(g9pHierarchy_t h)
{
	h->factors[0] = 1;
	for (int i = 1; i < h->numLevels; i++) {
		assert(h->dim1Ds[i] % h->dim1Ds[i - 1] == 0);
		h->factors[i] = h->dim1Ds[i] / h->dim1Ds[i - 1];
	}
}

static void
local_fillDimsFromFactors(g9pHierarchy_t h)
{
	for (int i = 1; i < h->numLevels; i++) {
		h->dim1Ds[i] = h->dim1Ds[i - 1] * h->factors[i];
	}
}

#if 0
static inline uint32_t *
local_getPartsPerMaskCell(const uint32_t numLevels,
                          const int8_t   maskLevel,
                          const uint32_t *factors)
{
	assert(numLevels != 0);
	assert((maskLevel > 0) && (numLevels > (uint32_t)maskLevel));

	uint32_t *partsPerMaskCell = xmalloc(sizeof(uint32_t) * numLevels);

	partsPerMaskCell[maskLevel] = 1;

	for (uint32_t i = (uint32_t)maskLevel + 1; i < numLevels; i++)
		partsPerMaskCell[i] = partsPerMaskCell[i - 1] * POW_NDIM(factors[i]);

	for (int i = maskLevel - 1; i >= 0; i--)
		partsPerMaskCell[i] = partsPerMaskCell[i + 1] * POW_NDIM(factors[i]);

	return partsPerMaskCell;
}

static inline int8_t
local_limitLevel(const int8_t level,
                 const int8_t minLevel,
                 const int8_t maxLevel)
{
	if (level < minLevel)
		return minLevel;

	if (level > maxLevel)
		return maxLevel;

	return level;
}

static inline uint64_t
local_cellsToParticles(const uint64_t cells,
                       const int8_t   curLevel,
                       const int8_t   maskLevel,
                       const uint32_t convFactor)
{
	if (curLevel < maskLevel) {
		assert(cells % convFactor == 0);
		return cells / convFactor;
	} else {
		return cells * convFactor;
	}
}

#endif
