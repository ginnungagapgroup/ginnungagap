// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lare_tests.h"
#include "lare.h"
#include <stdio.h>
#include <string.h>
#include "../libgrid/gridPoint.h"
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "lare_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
lare_new_test(void)
{
	bool              hasPassed = true;
	lare_t            lare;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++)
		dims[i] = 2048;
	lare = lare_new(dims, 30);
	for (int i = 0; i < NDIM; i++) {
		if (lare->dims[i] != 2048)
			hasPassed = false;
	}
	if (lare->numElements != 30)
		hasPassed = false;
	if (lare->elements[0][0] != 2048)
		hasPassed = false;
	if (lare->elements[29][NDIM - 1] != 2048)
		hasPassed = false;
	lare_del(&lare);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
lare_del_test(void)
{
	bool              hasPassed = true;
	lare_t            lare;
	gridPointUint32_t dims;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++)
		dims[i] = 2048;
	lare = lare_new(dims, 30);
	lare_del(&lare);
	if (lare != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
lare_getElement_test(void)
{
	bool              hasPassed = true;
	lare_t            lare;
	gridPointUint32_t dims;
	gridPointUint32_t element;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++)
		dims[i] = 2048;
	lare = lare_new(dims, 30);
	for (int i = 0; i < NDIM; i++)
		lare->elements[0][i] = 45;

	lare_getElement(lare, element, 0);

	for (int i = 0; i < NDIM; i++) {
		if (element[i] != lare->elements[0][i])
			hasPassed = false;
	}

	lare_del(&lare);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
lare_setElement_test(void)
{
	bool              hasPassed = true;
	lare_t            lare;
	gridPointUint32_t dims;
	gridPointUint32_t element, elementCheck;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++)
		dims[i] = 2048;
	lare = lare_new(dims, 30);
	for (int i = 0; i < NDIM; i++)
		element[i] = 45;

	lare_setElement(lare, element, 0);
	lare_setElement(lare, element, 15);
	lare_setElement(lare, element, 29);

	lare_getElement(lare, elementCheck, 0);
	for (int i = 0; i < NDIM; i++) {
		if (elementCheck[i] != element[i])
			hasPassed = false;
	}
	lare_getElement(lare, elementCheck, 15);
	for (int i = 0; i < NDIM; i++) {
		if (elementCheck[i] != element[i])
			hasPassed = false;
	}
	lare_getElement(lare, elementCheck, 29);
	for (int i = 0; i < NDIM; i++) {
		if (elementCheck[i] != element[i])
			hasPassed = false;
	}

	lare_del(&lare);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* lare_setElement_test */

extern bool
lare_addElement_test(void)
{
	bool              hasPassed = true;
	lare_t            lare;
	gridPointUint32_t dims;
	gridPointUint32_t element, elementCheck;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++)
		dims[i] = 2048;
	lare = lare_new(dims, 30);
	for (int i = 0; i < NDIM; i++)
		element[i] = 45;

	lare_addElement(lare, element);

	if (lare->numElements != 31)
		hasPassed = false;
	lare_getElement(lare, elementCheck, 30);
	for (int i = 0; i < NDIM; i++) {
		if (elementCheck[i] != element[i])
			hasPassed = false;
	}

	lare_del(&lare);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* lare_addElement_test */

/*--- Implementations of local functions --------------------------------*/
