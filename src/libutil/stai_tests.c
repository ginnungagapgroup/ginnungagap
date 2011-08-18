// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  stai_tests.c
 * @ingroup  libutilMiscStai
 * @brief  Implementation of the test functions of stai.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "stai_tests.h"
#include "stai.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "stai_adt.h"


/*--- Local structure ---------------------------------------------------*/

/// @cond
struct stai_test_struct {
	char   name[7];
	double value;
};

typedef struct stai_test_struct *stai_test_t;
/// @endcond


/*--- Prototypes of local functions -------------------------------------*/

/// @cond
static stai_test_t
local_getTestData(void);


/// @endcond


/*--- Implementations of exported functions -----------------------------*/
extern bool
stai_new_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai;
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(testData->name, 7,
	                    (char *)&(testData[1].name)
	                    - (char *)&(testData[0].name));
	if (stai->base != testData->name)
		hasPassed = false;
	if (stai->sizeOfElementInBytes != 7)
		hasPassed = false;
	if (stai->strideInBytes != sizeof(struct stai_test_struct))
		hasPassed = false;
	stai_del(&stai);

	stai = stai_new(&(testData->value), sizeof(double),
	                (char *)&(testData[1].value)
	                - (char *)&(testData[0].value));
	if (stai->base != &(testData->value))
		hasPassed = false;
	if (stai->sizeOfElementInBytes != sizeof(double))
		hasPassed = false;
	if (stai->strideInBytes != sizeof(struct stai_test_struct))
		hasPassed = false;
	stai_del(&stai);

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_new_test */

extern bool
stai_cloneWithDifferentBase_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai, staiClone;
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(testData->name, 7,
	                    (char *)&(testData[1].name)
	                    - (char *)&(testData[0].name));
	staiClone = stai_cloneWithDifferentBase(stai, &(testData[1].name));
	if (staiClone->base != &(testData[1].name))
		hasPassed = false;
	if (staiClone->sizeOfElementInBytes != stai->sizeOfElementInBytes)
		hasPassed = false;
	if (staiClone->strideInBytes != stai->strideInBytes)
		hasPassed = false;
	stai_del(&stai);
	stai_del(&staiClone);

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_cloneWithDifferentBase_test */

extern bool
stai_del_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai;
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(testData->name, 7,
	                    (char *)&(testData[1].name)
	                    - (char *)&(testData[0].name));
	stai_del(&stai);
	if (stai != NULL)
		hasPassed = false;

	stai = stai_new(&(testData->value), sizeof(double),
	                (char *)&(testData[1].value)
	                - (char *)&(testData[0].value));
	stai_del(&stai);
	if (stai != NULL)
		hasPassed = false;

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_del_test */

extern bool
stai_setElement_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai;
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(testData->name, 7,
	                    (char *)&(testData[1].name)
	                    - (char *)&(testData[0].name));
	stai_setElement(stai, 31, "HuhuDu");
	if (strncmp("HuhuDu", testData[31].name, 6) != 0)
		hasPassed = false;
	stai_setElement(stai, 32,
	                "This is a way too long string but it should work.");
	if (strncmp("This is a way too long string but it should work.",
	            testData[32].name, 7) != 0)
		hasPassed = false;
	if (strncmp("test33", testData[33].name, 7) != 0)
		hasPassed = false;
	stai_del(&stai);

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_setElement_test */

extern bool
stai_setElementsMulti_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai;
	double      newData[]      = {100.0, 101.0, 102.0, 103.0};
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(&(testData->value), sizeof(double),
	                    sizeof(struct stai_test_struct));
	stai_setElementsMulti(stai, 5, newData, 4);
	if (islessgreater(testData[5].value, newData[0]))
		hasPassed = false;
	if (islessgreater(testData[6].value, newData[1]))
		hasPassed = false;
	if (islessgreater(testData[7].value, newData[2]))
		hasPassed = false;
	if (islessgreater(testData[8].value, newData[3]))
		hasPassed = false;
	stai_del(&stai);

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_setElementsMulti_test */

extern bool
stai_getElement_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai;
	double      newData;
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(&(testData->value), sizeof(double),
	                    sizeof(struct stai_test_struct));
	stai_getElement(stai, 49, &newData);
	if (islessgreater(testData[49].value, newData))
		hasPassed = false;
	stai_del(&stai);

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_getElement_test */

extern bool
stai_getElementsMulti_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai;
	double      newData[4];
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(&(testData->value), sizeof(double),
	                    sizeof(struct stai_test_struct));
	stai_getElementsMulti(stai, 33, newData, 4);
	if (islessgreater(testData[33].value, newData[0]))
		hasPassed = false;
	if (islessgreater(testData[34].value, newData[1]))
		hasPassed = false;
	if (islessgreater(testData[35].value, newData[2]))
		hasPassed = false;
	if (islessgreater(testData[36].value, newData[3]))
		hasPassed = false;
	stai_del(&stai);

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_getElementsMulti_test */

extern bool
stai_isLinear_test(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
	stai_test_t testData;
	stai_t      stai;
	double      newData[4];
#ifdef ENABLE_XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	testData = local_getTestData();

	stai     = stai_new(&(testData->value), sizeof(double),
	                    sizeof(struct stai_test_struct));
	if (stai_isLinear(stai))
		hasPassed = false;
	stai_del(&stai);

	stai = stai_new(testData, sizeof(struct stai_test_struct),
	                sizeof(struct stai_test_struct));
	if (!stai_isLinear(stai))
		hasPassed = false;
	stai_del(&stai);

	xfree(testData);
#ifdef ENABLE_XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* stai_isLinear_test */

/*--- Implementations of local functions --------------------------------*/

/// @cond
static stai_test_t
local_getTestData(void)
{
	stai_test_t testData;

	testData = xmalloc(sizeof(struct stai_test_struct) * 50);
	for (int i = 0; i < 50; i++) {
		sprintf(testData[i].name, "test%02i", i);
		testData[i].value = (double)i;
	}

	return testData;
}

/// @endcond
