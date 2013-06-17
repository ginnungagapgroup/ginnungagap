// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pMaskShapelet_tests.c
 * @ingroup  libg9pMaskShapeletTest
 * @brief  Implements the shapelet tests for @ref libg9pMaskShapelet.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMaskShapelet_tests.h"
#include "g9pMaskShapelet.h"
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "g9pMaskShapelet_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pMaskShapelet_verifyShapelet(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	g9pMaskShapelet_t sl;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	sl = g9pMaskShapelet_new(INT8_C(2), INT8_C(10));

	if (g9pMaskShapelet_getLowLevel(sl) != INT8_C(2))
		hasPassed = false;
	if (g9pMaskShapelet_getHighLevel(sl) != INT8_C(10))
		hasPassed = false;
	if (g9pMaskShapelet_getExtent(sl) != INT8_C(8))
		hasPassed = false;
	if (g9pMaskShapelet_getDim1D(sl) != INT8_C(17))
		hasPassed = false;

	const int8_t *data = g9pMaskShapelet_getData(sl);

	if (data[0] != INT8_C(2))
		hasPassed = false;

	for (int k = 0; k <= 8; k++) {
#if (NDIM == 3)
		if (data[k * (1 + 17 + 17 * 17)] != k + 2)
#else
		if (data[k * (1 + 17)] != k + 2)
#endif
		{
			hasPassed = false;
		}
	}
	for (int k = 9; k < 17; k++) {
#if (NDIM == 3)
		if (data[k * (1 + 17 + 17 * 17)] != 16 - k + 2)
#else
		if (data[k * (1 + 17)] != 16 - k + 2)
#endif
		{
			hasPassed = false;
		}
	}
	for (int k = 0; k <= 8; k++) {
#if (NDIM == 3)
		if (data[8 + k * 17 + 8 * 17 * 17] != k + 2)
#else
		if (data[8 + k * 17 + 8] != k + 2)
#endif
			hasPassed = false;
	}
	for (int k = 9; k < 17; k++) {
#if (NDIM == 3)
		if (data[8 + k * 17 + 8 * 17 * 17] != 16 - k + 2)
#else
		if (data[8 + k * 17 + 8] != 16 - k + 2)
#endif
			hasPassed = false;
	}

	g9pMaskShapelet_del(&sl);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskShapelet_verifyShapelet */

/*--- Implementations of local functions --------------------------------*/
