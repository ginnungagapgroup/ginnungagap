// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include "partBunch.h"
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "../libdata/dataVar.h"
#include "../libutil/xmem.h"
#include "../libutil/varArr.h"


/*--- Implemention of main structure ------------------------------------*/
#include "partBunch_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern partBunch_t
partBunch_new(dataParticle_t desc, uint64_t initialNumParts)
{
	partBunch_t bunch;

	bunch               = xmalloc(sizeof(struct partBunch_struct));

	bunch->desc         = dataParticle_getRef(desc);
	bunch->numParticles = initialNumParts;
	bunch->data         = varArr_new(dataParticle_getNumVars(desc));
	for (int i = 0; i < dataParticle_getNumVars(desc); i++)
		(void)varArr_insert(bunch->data, NULL);
	bunch->isAllocated = false;

	return bunch;
}

extern void
partBunch_del(partBunch_t *bunch)
{
	assert(bunch != NULL && *bunch != NULL);

	if (partBunch_isAllocated(*bunch))
		partBunch_freeMem(*bunch);
	dataParticle_del(&((*bunch)->desc));
	varArr_del(&((*bunch)->data));
	xfree(*bunch);

	*bunch = NULL;
}

extern void
partBunch_allocMem(partBunch_t bunch)
{
	assert(bunch != NULL);
	assert(!partBunch_isAllocated(bunch));

	if (bunch->numParticles > UINT64_C(0)) {
		for (int i = 0; i < varArr_getLength(bunch->data); i++) {
			dataVar_t var = dataParticle_getVarHandle(bunch->desc, i);
			(void)varArr_replace(bunch->data, i,
			                     dataVar_getMemory(var, bunch->numParticles));
		}
	}

	bunch->isAllocated = true;
}

extern void
partBunch_freeMem(partBunch_t bunch)
{
	assert(bunch != NULL);
	assert(partBunch_isAllocated(bunch));

	if (bunch->numParticles > UINT64_C(0)) {
		for (int i = 0; i < varArr_getLength(bunch->data); i++) {
			void      *old;
			dataVar_t var = dataParticle_getVarHandle(bunch->desc, i);
			old = varArr_replace(bunch->data, i, NULL);
			dataVar_freeMemory(var, old);
		}
	}

	bunch->isAllocated = false;
}

extern void
partBunch_resize(partBunch_t bunch, uint64_t numParticles)
{
	assert(bunch != NULL);

	if (numParticles != partBunch_getNumParticles(bunch)) {
		bool wasAllocated;
		if ((wasAllocated = partBunch_isAllocated(bunch)))
			partBunch_freeMem(bunch);
		bunch->numParticles = numParticles;
		if (wasAllocated)
			partBunch_allocMem(bunch);
	}
}

extern bool
partBunch_isAllocated(const partBunch_t bunch)
{
	assert(bunch != NULL);

	return bunch->isAllocated ? true : false;
}

extern uint64_t
partBunch_getNumParticles(const partBunch_t bunch)
{
	assert(bunch != NULL);

	return bunch->numParticles;
}

/*--- Implementations of local functions --------------------------------*/
