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
lare_getDims(const lare_t lare, gridPointUint32_t dims)
{
	assert(lare != NULL);
	assert(dims != NULL);

	for (int i = 0; i < NDIM; i++)
		dims[i] = lare->dims[i];
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

extern void
lare_getCenter(lare_t lare, float *center) {
	gridPointUint32_t point;
	int m0, m0addr, m0end, m0curr, m0curraddr, m0currend, idim, iter, ik, i;
	lare_getDims(lare, point);
	uint32_t nline = point[0];
	int8_t	*mask1d = xmalloc(sizeof(int8_t) * nline);
	uint32_t numLare = lare_getNumElements(lare);
	double Box = 1.0;
	
	for(idim=0;idim<3;idim++) {
		for(ik=0;ik<nline;ik++) mask1d[ik]=0;
		for(i=0;i<numLare;i++) {
			lare_getElement(lare, point, i);
			mask1d[point[idim]]=1;
		}
		
		m0=-1;
	    m0addr=-1;
	    m0end=-1;
	    m0curr=-1;
	    m0curraddr=0;
	    m0currend=0;
	    for(iter=1;iter<=2;iter++){
			for(ik=0;ik<nline;ik++) {
				if(mask1d[ik]==0 && ik!=m0currend+1) {
			       m0currend=ik-1;
			       m0curraddr=ik;
			       m0curr=0;
			     }
			     if(mask1d[ik]==0 && ik==m0currend+1) {
			       m0currend=m0currend+1;
			       m0curr=m0curr+1;
			     }
			     if(mask1d[ik]==1 && ik==m0currend+1 && m0curr>m0) {
			       m0=m0curr;
			       m0addr=m0curraddr;
			       m0end=m0currend;
			     }
			}
			if(m0currend==nline-1) m0currend=-1;
		}
		if(m0end>m0addr) m0addr=m0addr+nline;
 	    center[idim] = (m0addr+m0end+2)*Box/2/nline;
	    if(center[idim]>Box) center[idim]=center[idim]-Box;
	}
}

/*--- Implementations of local functions --------------------------------*/
static inline void
local_setInvalidElement(lare_t lare, uint32_t idxOfElement)
{
	for (int i = 0; i < NDIM; i++)
		lare->elements[idxOfElement][i] = lare->dims[i];
}
