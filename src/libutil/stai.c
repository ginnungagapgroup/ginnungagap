// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file stai.c
 * @ingroup libutilMiscStai
 * @brief  This file provides the implementation of stridden arrays.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "stai.h"
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "stai_adt.h"


/*--- Implementations of exported functions -----------------------------*/
extern stai_t
stai_new(void         *base,
         unsigned int sizeOfElementInBytes,
         unsigned int strideInBytes)
{
	assert(base != NULL);
	assert(sizeOfElementInBytes > 0);
	assert(strideInBytes > 0);

	stai_t stai = xmalloc(sizeof(struct stai_struct));

	stai->base                 = base;
	stai->sizeOfElementInBytes = sizeOfElementInBytes;
	stai->strideInBytes        = strideInBytes;

	return stai;
}

extern stai_t
stai_cloneWithDifferentBase(const stai_t stai, void *newBase)
{
	assert(stai != NULL);
	assert(newBase != NULL);

	return stai_new(newBase,
	                stai->sizeOfElementInBytes,
	                stai->strideInBytes);
}

extern void
stai_del(stai_t *stai)
{
	assert(stai != NULL && *stai != NULL);

	xfree(*stai);

	*stai = NULL;
}

extern void *
stai_getBase(const stai_t stai)
{
	assert(stai != NULL);

	return stai->base;
}

extern int
stai_getSizeOfElementInBytes(const stai_t stai)
{
	assert(stai != NULL);

	return stai->sizeOfElementInBytes;
}

extern int
stai_getStrideInBytes(const stai_t stai)
{
	assert(stai != NULL);

	return stai->strideInBytes;
}

extern void
stai_setElement(const stai_t stai, uint64_t pos, const void *element)
{
	assert(stai != NULL);
	assert(element != NULL);

	memcpy((char *)(stai->base) + pos * stai->strideInBytes,
	       element, stai->sizeOfElementInBytes);
}

extern void
stai_setElementsMulti(const stai_t stai,
                      uint64_t     pos,
                      const void   *elements,
                      uint64_t     numElements)
{
	assert(stai != NULL || numElements == UINT64_C(0));
	assert(elements != NULL || numElements == UINT64_C(0));

	for (uint64_t i = 0; i < numElements; i++)
		stai_setElement(stai, pos + i, ((const char *)elements
		                                + stai->sizeOfElementInBytes * i));
}

extern void
stai_getElement(const stai_t stai, uint64_t pos, void *element)
{
	assert(stai != NULL);
	assert(element != NULL);

	memcpy(element,
	       (char *)(stai->base) + pos * stai->strideInBytes,
	       stai->sizeOfElementInBytes);
}

extern void
stai_getElementsMulti(const stai_t stai,
                      uint64_t     pos,
                      void         *elements,
                      uint64_t     numElements)
{
	assert(stai != NULL || numElements == UINT64_C(0));
	assert(elements != NULL || numElements == UINT64_C(0));

	for (uint64_t i = UINT64_C(0); i < numElements; i++)
		stai_getElement(stai, pos + i,
		                ((char *)elements + stai->sizeOfElementInBytes * i));
}

extern void
stai_rebase(stai_t stai, int64_t offset)
{
	assert(stai != NULL);

	if (offset == INT64_C(0))
		return;

	stai->base = (char *)(stai->base) + offset * stai->strideInBytes;
}

extern bool
stai_isLinear(const stai_t stai)
{
	assert(stai != NULL);

	return stai->sizeOfElementInBytes == stai->strideInBytes ? true : false;
}
