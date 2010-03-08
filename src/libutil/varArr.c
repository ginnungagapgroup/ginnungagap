// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "varArr.h"
#include "xmem.h"
#include <assert.h>


/*--- Implemention of main structure ------------------------------------*/
#include "varArr_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern varArr_t
varArr_new(int numElements)
{
	varArr_t arr;

	assert(numElements >= 0);

	arr = xmalloc(sizeof(struct varArr_struct));
	if (numElements > 0) {
		arr->elements     = xmalloc(sizeof(void *) * numElements);
		arr->numAllocated = numElements;
		arr->increment    = numElements / 20;
		arr->increment    = arr->increment < VARARR_MIN_INCR
		                    ? VARARR_MIN_INCR : arr->increment;
	} else {
		arr->elements     = NULL;
		arr->numAllocated = 0;
		arr->increment    = VARARR_MIN_INCR;
	}
	arr->numUsed = 0;

	return arr;
}

extern void
varArr_del(varArr_t *arr)
{
	assert(arr != NULL && *arr != NULL);
	assert((*arr)->numUsed == 0);

	if ((*arr)->elements != NULL)
		xfree((*arr)->elements);
	xfree(*arr);
	*arr = NULL;
}

extern int
varArr_getLength(varArr_t arr)
{
	assert(arr != NULL);

	return arr->numUsed;
}

extern int
varArr_insert(varArr_t arr, void *element)
{
	assert(arr != NULL);
	assert(element != NULL);

	if (arr->numUsed == arr->numAllocated)
		arr->elements = xrealloc(arr->elements,
		                         sizeof(void *) * (arr->numUsed
		                                           + arr->increment));
	arr->elements[arr->numUsed] = element;
	arr->numUsed++;

	return arr->numUsed - 1;
}

extern void *
varArr_remove(varArr_t arr, int numElement)
{
	void *removedElement;

	assert(arr != NULL);
	assert(numElement >= 0 && numElement < arr->numUsed);

	removedElement = arr->elements[numElement];
	for (int i = numElement; i < arr->numUsed - 1; i++)
		arr->elements[i] = arr->elements[i + 1];
	arr->numUsed--;
	if (arr->numUsed + arr->increment < arr->numAllocated)
		arr->elements = xrealloc(arr->elements,
		                         sizeof(void *) * (arr->numUsed));

	return removedElement;
}

extern void *
varArr_getElementHandle(varArr_t arr, int numElement)
{
	assert(arr != NULL);
	assert(numElement >= 0 && numElement < arr->numUsed);

	return arr->elements[numElement];
}

/*--- Implementations of local functions --------------------------------*/
