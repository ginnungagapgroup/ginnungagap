// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pIDGenerator.c
 * @ingroup  GROUP
 * @brief  SHORT DESC
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pIDGenerator.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/lIdx.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pIDGenerator_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static g9pIDGenerator_t
local_allocate(void);


/*--- Implementations of exported functions -----------------------------*/
extern g9pIDGenerator_t
g9pIDGenerator_new(g9pHierarchy_t h, uint8_t idLevel)
{
	assert(h != NULL);
	assert(idLevel < g9pHierarchy_getNumLevels(h));

	g9pIDGenerator_t idGen = local_allocate();

	idGen->h       = h;
	idGen->idLevel = idLevel;
	const uint32_t tmp = g9pHierarchy_getDim1DAtLevel(h, idLevel);
	for (int i = 0; i < NDIM; i++)
		idGen->idDims[i] = tmp;
	idGen->maxID = POW_NDIM((g9pID_t)(tmp));

	return g9pIDGenerator_getRef(idGen);
}

extern g9pIDGenerator_t
g9pIDGenerator_getRef(g9pIDGenerator_t idGen)
{
	assert(idGen != NULL);

	refCounter_ref(&(idGen->refCounter));

	return idGen;
}

extern void
g9pIDGenerator_del(g9pIDGenerator_t *idGen)
{
	assert(idGen != NULL && *idGen != NULL);

	if (refCounter_deref(&((*idGen)->refCounter))) {
		g9pHierarchy_del(&((*idGen)->h));
		xfree(*idGen);
	}

	*idGen = NULL;
}

extern g9pID_t
g9pIDGenerator_getMaxID(const g9pIDGenerator_t idGen)
{
	assert(idGen != NULL);

	return idGen->maxID;
}

extern g9pID_t
g9pIDGenerator_calcID(const g9pIDGenerator_t  idGen,
                      const gridPointUint32_t coord,
                      const uint8_t           coordLevel)
{
	assert(idGen != NULL);
	assert(coordLevel <= idGen->idLevel);


	if (coordLevel != idGen->idLevel) {
		gridPointUint32_t tmpCoord;
		uint32_t          fac
		    = g9pHierarchy_getFactorBetweenLevel(idGen->h,
		                                         coordLevel,
		                                         idGen->idLevel);
		for (int i = 0; i < NDIM; i++)
			tmpCoord[i] = coord[i] * fac;
		return (g9pID_t)lIdx_fromCoordNdim(tmpCoord, idGen->idDims);
	} else {
		return (g9pID_t)lIdx_fromCoordNdim(coord, idGen->idDims);
	}
}

extern void
g9pIDGenerator_calcCoord(const g9pIDGenerator_t idGen,
                         g9pID_t          id,
                         gridPointUint32_t      coord,
                         const uint8_t          coordLevel)
{
	assert(idGen != NULL);
	assert(coordLevel <= idGen->idLevel);

	while (id >= idGen->maxID)
		id -= idGen->maxID;

	lIdx_toCoordNdim(id, idGen->idDims, coord);
	if (coordLevel != idGen->idLevel) {
		uint32_t fac = g9pHierarchy_getFactorBetweenLevel(idGen->h,
		                                                  coordLevel,
		                                                  idGen->idLevel);
		for (int i = 0; i < NDIM; i++)
			coord[i] /= fac;
	}
}

/*--- Implementations of local functions --------------------------------*/

static g9pIDGenerator_t
local_allocate(void)
{
	g9pIDGenerator_t idGen = xmalloc(sizeof(struct g9pIDGenerator_struct));

	refCounter_init(&(idGen->refCounter));

	return idGen;
}
