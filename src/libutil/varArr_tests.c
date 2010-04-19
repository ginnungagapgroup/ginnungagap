// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "varArr_tests.h"
#include "varArr.h"
#include <stdio.h>
#ifdef XMEM_TRACK_MEM
#  include "xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Implemention of main structure ------------------------------------*/
#include "varArr_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
varArr_new_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	arr = varArr_new(0);
	if (arr->numAllocated != 0)
		hasPassed = false;
	if (arr->increment != VARARR_MIN_INCR)
		hasPassed = false;
	if (arr->elements != NULL)
		hasPassed = false;
	if (arr->numUsed != 0)
		hasPassed = false;
	varArr_del(&arr);

	arr = varArr_new(5);
	if (arr->numAllocated != 5)
		hasPassed = false;
	if (arr->increment != VARARR_MIN_INCR)
		hasPassed = false;
	if (arr->elements == NULL)
		hasPassed = false;
	if (arr->numUsed != 0)
		hasPassed = false;
	varArr_del(&arr);

	arr = varArr_new(12000);
	if (arr->numAllocated != 12000)
		hasPassed = false;
	if (arr->increment != 600)
		hasPassed = false;
	if (arr->elements == NULL)
		hasPassed = false;
	if (arr->numUsed != 0)
		hasPassed = false;
	varArr_del(&arr);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* varArr_new_test */

extern bool
varArr_del_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	arr = varArr_new(12000);
	varArr_del(&arr);

	if (arr != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
varArr_getLength_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	arr = varArr_new(12000);

	if (varArr_getLength(arr) != 0)
		hasPassed = false;
	varArr_insert(arr, &arr);
	if (varArr_getLength(arr) != 1)
		hasPassed = false;
	(void)varArr_remove(arr, 0);
	if (varArr_getLength(arr) != 0)
		hasPassed = false;

	varArr_del(&arr);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
varArr_insert_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	arr = varArr_new(5);

	for (int i = 0; i < 4; i++)
		varArr_insert(arr, &arr);
	if (arr->numAllocated != 5)
		hasPassed = false;
	varArr_insert(arr, &arr);
	varArr_insert(arr, &arr);
	if (arr->numAllocated != 5 + arr->increment)
		hasPassed = false;
	while (varArr_getLength(arr) != 0)
		varArr_remove(arr, 0);
	if (varArr_getLength(arr) != 0)
		hasPassed = false;

	varArr_del(&arr);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
varArr_remove_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	arr = varArr_new(5);

	for (int i = 0; i < 6; i++)
		varArr_insert(arr, &arr);
	if (varArr_remove(arr, 5) != &arr)
		hasPassed = false;
	if (arr->numAllocated != 5 + arr->increment)
		hasPassed = false;
	(void)varArr_remove(arr, 2);
	(void)varArr_remove(arr, 0);
	if (arr->numUsed != 3)
		hasPassed = false;
	while (varArr_getLength(arr) != 0)
		varArr_remove(arr, 0);

	varArr_del(&arr);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
varArr_replace_test(void)
{
	bool     hasPassed    = true;
	int      testElement1 = 999;
	int      testElement2 = 42;
	varArr_t arr;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	arr = varArr_new(1);
	varArr_insert(arr, &testElement1);
	if (varArr_replace(arr, 0, &testElement2) != &testElement1)
		hasPassed = false;
	if (varArr_remove(arr, 0) != &testElement2)
		hasPassed = false;

	varArr_del(&arr);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
varArr_getElementHandle_test(void)
{
	bool     hasPassed   = true;
	varArr_t arr;
	int      testElement = 42;
	int      *handle;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	arr    = varArr_new(3);
	varArr_insert(arr, &testElement);
	handle = varArr_getElementHandle(arr, 0);
	if ((handle != &testElement) || (*handle != testElement))
		hasPassed = false;
	varArr_remove(arr, 0);
	varArr_del(&arr);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
