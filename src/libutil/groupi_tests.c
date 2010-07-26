// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "groupi_tests.h"
#include "groupi.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "groupi_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
groupi_new_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	int      size;
	groupi_t groupi;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);
	if (size != 16)
		return true;

	groupi = groupi_new(3, MPI_COMM_WORLD, 123,
	                    GROUPI_MODE_BLOCK);
	switch (rank) {
		case 0:
			if (groupi->groupNumber != 0)
				hasPassed = false;
			if (groupi->prevProcess != -1)
				hasPassed = false;
			if (groupi->nextProcess != 1)
				hasPassed = false;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
			if (groupi->groupNumber != 0)
				hasPassed = false;
			if (groupi->prevProcess != rank - 1)
				hasPassed = false;
			if (groupi->nextProcess != rank + 1)
				hasPassed = false;
			break;
		case 5:
			if (groupi->groupNumber != 0)
				hasPassed = false;
			if (groupi->prevProcess != 4)
				hasPassed = false;
			if (groupi->nextProcess != -1)
				hasPassed = false;
			break;
		case 6:
			if (groupi->groupNumber != 1)
				hasPassed = false;
			if (groupi->prevProcess != -1)
				hasPassed = false;
			if (groupi->nextProcess != 7)
				hasPassed = false;
			break;
		case 7:
		case 8:
		case 9:
			if (groupi->groupNumber != 1)
				hasPassed = false;
			if (groupi->prevProcess != rank - 1)
				hasPassed = false;
			if (groupi->nextProcess != rank + 1)
				hasPassed = false;
			break;
		case 10:
			if (groupi->groupNumber != 1)
				hasPassed = false;
			if (groupi->prevProcess != 9)
				hasPassed = false;
			if (groupi->nextProcess != -1)
				hasPassed = false;
			break;
		case 11:
			if (groupi->groupNumber != 2)
				hasPassed = false;
			if (groupi->prevProcess != -1)
				hasPassed = false;
			if (groupi->nextProcess != 12)
				hasPassed = false;
			break;
		case 12:
		case 13:
		case 14:
			if (groupi->groupNumber != 2)
				hasPassed = false;
			if (groupi->prevProcess != rank - 1)
				hasPassed = false;
			if (groupi->nextProcess != rank + 1)
				hasPassed = false;
			break;
		case 15:
			if (groupi->groupNumber != 2)
				hasPassed = false;
			if (groupi->prevProcess != 14)
				hasPassed = false;
			if (groupi->nextProcess != -1)
				hasPassed = false;
			break;
		default:
			hasPassed = false;
	}
	groupi_del(&groupi);

	groupi = groupi_new(3, MPI_COMM_WORLD, 123,
	                    GROUPI_MODE_ROUNDROBIN);
	switch (rank) {
		case 0:
			if (groupi->groupNumber != 0)
				hasPassed = false;
			if (groupi->prevProcess != -1)
				hasPassed = false;
			if (groupi->nextProcess != 3)
				hasPassed = false;
			break;
		case 3:
		case 6:
		case 9:
		case 12:
			if (groupi->groupNumber != 0)
				hasPassed = false;
			if (groupi->prevProcess != rank - 3)
				hasPassed = false;
			if (groupi->nextProcess != rank + 3)
				hasPassed = false;
			break;
		case 15:
			if (groupi->groupNumber != 0)
				hasPassed = false;
			if (groupi->prevProcess != 12)
				hasPassed = false;
			if (groupi->nextProcess != -1)
				hasPassed = false;
			break;
		case 1:
			if (groupi->groupNumber != 1)
				hasPassed = false;
			if (groupi->prevProcess != -1)
				hasPassed = false;
			if (groupi->nextProcess != 4)
				hasPassed = false;
			break;
		case 4:
		case 7:
		case 10:
			if (groupi->groupNumber != 1)
				hasPassed = false;
			if (groupi->prevProcess != rank - 3)
				hasPassed = false;
			if (groupi->nextProcess != rank + 3)
				hasPassed = false;
			break;
		case 13:
			if (groupi->groupNumber != 1)
				hasPassed = false;
			if (groupi->prevProcess != 10)
				hasPassed = false;
			if (groupi->nextProcess != -1)
				hasPassed = false;
			break;
		case 2:
			if (groupi->groupNumber != 2)
				hasPassed = false;
			if (groupi->prevProcess != -1)
				hasPassed = false;
			if (groupi->nextProcess != 5)
				hasPassed = false;
			break;
		case 5:
		case 8:
		case 11:
			if (groupi->groupNumber != 2)
				hasPassed = false;
			if (groupi->prevProcess != rank - 3)
				hasPassed = false;
			if (groupi->nextProcess != rank + 3)
				hasPassed = false;
			break;
		case 14:
			if (groupi->groupNumber != 2)
				hasPassed = false;
			if (groupi->prevProcess != 11)
				hasPassed = false;
			if (groupi->nextProcess != -1)
				hasPassed = false;
			break;
		default:
			hasPassed = false;
	}
	groupi_del(&groupi);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
groupi_del_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	groupi_t groupi;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	groupi = groupi_new(3, MPI_COMM_WORLD, 123,
	                    GROUPI_MODE_BLOCK);
	groupi_del(&groupi);
	if (groupi != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
