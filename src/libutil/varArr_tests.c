// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "varArr_tests.h"
#include "varArr.h"
#include <stdio.h>


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

	return hasPassed ? true : false;
} /* varArr_new_test */

extern bool
varArr_del_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;

	printf("Testing %s... ", __func__);

	arr = varArr_new(12000);
	varArr_del(&arr);

	if (arr != NULL)
		hasPassed = false;

	return hasPassed ? true : false;
}

extern bool
varArr_getLength_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;

	printf("Testing %s... ", __func__);

	arr = varArr_new(12000);

	if (varArr_getLength(arr) != 0)
		hasPassed = false;
	varArr_insert(arr, &arr);
	if (varArr_getLength(arr) != 1)
		hasPassed = false;
	(void)varArr_remove(arr,0);
	if (varArr_getLength(arr) != 0)
		hasPassed = false;

	varArr_del(&arr);

	return hasPassed ? true : false;
}

extern bool
varArr_insert_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;

	printf("Testing %s... ", __func__);

	arr = varArr_new(5);

	for (int i=0; i<4; i++)
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

	return hasPassed ? true : false;
}

extern bool
varArr_remove_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;

	printf("Testing %s... ", __func__);

	arr = varArr_new(5);

	for (int i=0; i<6; i++)
		varArr_insert(arr, &arr);
	if (varArr_remove(arr, 5) != &arr)
		hasPassed = false;
	if (arr->numAllocated != 5+arr->increment)
		hasPassed = false;
	(void)varArr_remove(arr, 2);
	(void)varArr_remove(arr, 0);
	if (arr->numUsed != 3)
		hasPassed = false;
	while (varArr_getLength(arr) != 0)
		varArr_remove(arr, 0);

	varArr_del(&arr);

	return hasPassed ? true : false;
}

extern bool
varArr_getElementHandle_test(void)
{
	bool     hasPassed = true;
	varArr_t arr;

	printf("Testing %s... ", __func__);

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
