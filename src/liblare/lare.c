// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lare.h"
#include <assert.h>
#include <inttypes.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "../libgrid/gridPoint.h"


/*--- Implemention of main structure ------------------------------------*/
#include "lare_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static inline void
local_setInvalidElement(lare_t lare, uint32_t idxOfElement);


/*--- Implementations of exported functios ------------------------------*/
extern lare_t
lare_new(gridPointUint32_t dims, uint32_t numElements)
{
	lare_t lare;

	assert(numElements < UINT32_MAX);

	lare              = xmalloc(sizeof(struct lare_struct));
	lare->numElements = numElements;
	for (int i = 0; i < NDIM; i++)
		lare->dims[i] = dims[i];
	if (lare->numElements > 0) {
		lare->elements = xmalloc(sizeof(gridPointUint32_t) * numElements);
		for (uint32_t i = 0; i < numElements; i++) {
			local_setInvalidElement(lare, i);
		}
	} else {
		lare->elements = NULL;
	}

	return lare;
}

extern void
lare_del(lare_t *lare)
{
	assert(lare != NULL && *lare != NULL);

	xfree((*lare)->elements);
	xfree(*lare);

	*lare = NULL;
}

extern uint32_t
lare_getNumElements(const lare_t lare)
{
	assert(lare != NULL);

	return lare->numElements;
}

extern void
lare_getElement(const lare_t      lare,
                gridPointUint32_t element,
                uint32_t          idxOfElement)
{
	assert(lare != NULL);
	assert(idxOfElement < lare->numElements);

	for (int i = 0; i < NDIM; i++) {
		if (lare->elements[idxOfElement][i] >= lare->dims[i]) {
			fprintf(stderr,
			        "Error: lare element %" PRIu32 " is not valid.\n",
			        idxOfElement);
			diediedie(99823);
		}
	}

	for (int i = 0; i < NDIM; i++)
		element[i] = lare->elements[idxOfElement][i];
}

extern void
lare_setElement(lare_t                  lare,
                const gridPointUint32_t element,
                uint32_t                idxOfElement)
{
	assert(lare != NULL);
	assert(idxOfElement < lare->numElements);

	for (int i = 0; i < NDIM; i++) {
		if (element[i] >= lare->dims[i]) {
			fprintf(stderr, "Error: element exceeds dimensions.\n");
			diediedie(32105);
		}
	}

	for (int i = 0; i < NDIM; i++)
		lare->elements[idxOfElement][i] = element[i];
}

extern void
lare_addElement(lare_t                  lare,
                const gridPointUint32_t element)
{
	assert(lare != NULL);

	lare->elements = xrealloc(lare->elements,
	                          sizeof(gridPointUint32_t)
	                          * (lare->numElements + 1));
	local_setInvalidElement(lare, lare->numElements);
	lare->numElements++;
	lare_setElement(lare, element, lare->numElements - 1);
}

/*--- Implementations of local functions --------------------------------*/
static inline void
local_setInvalidElement(lare_t lare, uint32_t idxOfElement)
{
	for (int i = 0; i < NDIM; i++)
		lare->elements[idxOfElement][i] = lare->dims[i];
}
