// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  lIdx_tests.c
 * @ingroup  libutilMisc
 * @brief  Implements the tests for the lIdx module.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "lIdx_tests.h"
#include "lIdx.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef ENABLE_XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
lIdx_fromCoord2d_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint64_t idx, idxCorrect;
	uint32_t coord[2];
	uint32_t dims[2];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dims[0]    = 5;
	dims[1]    = 3;

	idxCorrect = 0;
	for (coord[1] = 0; coord[1] < dims[1]; coord[1]++) {
		for (coord[0] = 0; coord[0] < dims[0]; coord[0]++) {
			idx = lIdx_fromCoord2d(coord, dims);
			if (idx != idxCorrect)
				hasPassed = false;
			idxCorrect++;
		}
	}

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* lIdx_fromCoord2d_test */

extern bool
lIdx_toCoord2d_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint64_t idx;
	uint32_t coord[2], rtn[2];
	uint32_t dims[2];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dims[0] = 5;
	dims[1] = 3;

	idx     = 0;
	for (coord[1] = 0; coord[1] < dims[1]; coord[1]++) {
		for (coord[0] = 0; coord[0] < dims[0]; coord[0]++) {
			lIdx_toCoord2d(idx++, dims, rtn);
			if (rtn[0] != coord[0])
				hasPassed = false;
			if (rtn[1] != coord[1])
				hasPassed = false;
		}
	}

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* lIdx_toCoord2d_test */

extern bool
lIdx_fromCoord3d_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint64_t idx, idxCorrect;
	uint32_t coord[3];
	uint32_t dims[3];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dims[0]    = 5;
	dims[1]    = 3;
	dims[2]    = 7;

	idxCorrect = 0;
	for (coord[2] = 0; coord[2] < dims[2]; coord[2]++) {
		for (coord[1] = 0; coord[1] < dims[1]; coord[1]++) {
			for (coord[0] = 0; coord[0] < dims[0]; coord[0]++) {
				idx = lIdx_fromCoord3d(coord, dims);
				if (idx != idxCorrect)
					hasPassed = false;
				idxCorrect++;
			}
		}
	}

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* lIdx_fromCoord3d_test */

extern bool
lIdx_toCoord3d_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint64_t idx;
	uint32_t coord[3], rtn[3];
	uint32_t dims[3];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dims[0] = 5;
	dims[1] = 3;
	dims[2] = 4;

	idx     = 0;
	for (coord[2] = 0; coord[2] < dims[2]; coord[2]++) {
		for (coord[1] = 0; coord[1] < dims[1]; coord[1]++) {
			for (coord[0] = 0; coord[0] < dims[0]; coord[0]++) {
				lIdx_toCoord3d(idx++, dims, rtn);
				if (rtn[0] != coord[0])
					hasPassed = false;
				if (rtn[1] != coord[1])
					hasPassed = false;
				if (rtn[2] != coord[2])
					hasPassed = false;
			}
		}
	}

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* lIdx_toCoord3d_test */

extern bool
lIdx_fromCoordNd_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint64_t idx, idxCorrect;
	uint32_t coord[5];
	uint32_t dims[5];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dims[0]    = 5;
	dims[1]    = 3;
	dims[2]    = 7;
	dims[3]    = 1;
	dims[4]    = 2;

	idxCorrect = 0;
	for (coord[4] = 0; coord[4] < dims[4]; coord[4]++) {
		for (coord[3] = 0; coord[3] < dims[3]; coord[3]++) {
			for (coord[2] = 0; coord[2] < dims[2]; coord[2]++) {
				for (coord[1] = 0; coord[1] < dims[1]; coord[1]++) {
					for (coord[0] = 0; coord[0] < dims[0]; coord[0]++) {
						idx = lIdx_fromCoordNd(coord, dims, 5);
						if (idx != idxCorrect)
							hasPassed = false;
						idxCorrect++;
					}
				}
			}
		}
	}

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* lIdx_fromCoordNd_test */

extern bool
lIdx_toCoordNd_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	uint64_t idx;
	uint32_t coord[5], rtn[5];
	uint32_t dims[5];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	dims[0] = 5;
	dims[1] = 3;
	dims[2] = 4;
	dims[3] = 1;
	dims[4] = 9;

	idx     = 0;
	for (coord[4] = 0; coord[4] < dims[4]; coord[4]++) {
		for (coord[3] = 0; coord[3] < dims[3]; coord[3]++) {
			for (coord[2] = 0; coord[2] < dims[2]; coord[2]++) {
				for (coord[1] = 0; coord[1] < dims[1]; coord[1]++) {
					for (coord[0] = 0; coord[0] < dims[0]; coord[0]++) {
						lIdx_toCoordNd(idx++, dims, 5, rtn);
						if (rtn[0] != coord[0])
							hasPassed = false;
						if (rtn[1] != coord[1])
							hasPassed = false;
						if (rtn[2] != coord[2])
							hasPassed = false;
						if (rtn[3] != coord[3])
							hasPassed = false;
						if (rtn[4] != coord[4])
							hasPassed = false;
					}
				}
			}
		}
	}

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}         /* lIdx_toCoordNd_test */

/*--- Implementations of local functions --------------------------------*/
