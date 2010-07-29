// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "groupi.h"
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <mpi.h>
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "groupi_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_calcGroupDistribCommon(groupi_t groupi,
                             int      *rank,
                             int      *size,
                             int      *numLargeGroups);

static void
local_calcGroupiDistribBlock(groupi_t groupi);

static void
local_calcGroupiDistribRoundRobin(groupi_t groupi);


/*--- Implementations of exported functios ------------------------------*/
extern groupi_t
groupi_new(int           numGroups,
           MPI_Comm      mpiComm,
           int           mpiTag,
           groupi_mode_t mode)
{
	groupi_t groupi;

	assert(numGroups > 0);
	assert(mpiComm != MPI_COMM_NULL);
	assert(mpiTag >= 0 && mpiTag < 32767);

	groupi                  = xmalloc(sizeof(struct groupi_struct));
	groupi->numGroups       = numGroups;
	groupi->mpiComm         = mpiComm;
	groupi->mpiTag          = mpiTag;
	groupi->isAcquired      = false;
	groupi->acquireFunc     = NULL;
	groupi->acquireFuncData = NULL;
	groupi->releaseFunc     = NULL;
	groupi->releaseFuncData = NULL;

	if (mode == GROUPI_MODE_BLOCK)
		local_calcGroupiDistribBlock(groupi);
	else
		local_calcGroupiDistribRoundRobin(groupi);

	return groupi;
}

extern void
groupi_del(groupi_t *groupi)
{
	assert(groupi != NULL && *groupi != NULL);

	if (groupi_isAcquired(*groupi))
		groupi_release(*groupi);

	xfree(*groupi);

	*groupi = NULL;
}

extern void
groupi_registerAcquireFunc(groupi_t            groupi,
                           groupiAcquireFunc_t acquireFunc,
                           void                *acquireFuncData)
{
	assert(groupi != NULL);
	assert(acquireFunc != NULL);

	groupi->acquireFunc     = acquireFunc;
	groupi->acquireFuncData = acquireFuncData;
}

extern void
groupi_registerReleaseFunc(groupi_t            groupi,
                           groupiReleaseFunc_t releaseFunc,
                           void                *releaseFuncData)
{
	assert(groupi != NULL);
	assert(acquireFunc != NULL);

	groupi->releaseFunc     = releaseFunc;
	groupi->releaseFuncData = releaseFuncData;
}

extern int
groupi_getRankInGroup(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->rankInGroup;
}

extern int
groupi_getSizeOfGroup(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->sizeOfGroup;
}

extern int
groupi_getNumGroups(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->numGroups;
}

extern bool
groupi_isAcquired(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->isAcquired ? true : false;
}

extern void *
groupi_acquire(groupi_t groupi)
{
	void *rtn = NULL;

	assert(groupi != NULL);

	if (groupi->prevProcess >= 0) {
		int        buf;
		MPI_Status status;

		MPI_Recv(&buf, 1, MPI_INT, groupi->prevProcess,
		         groupi->mpiTag, groupi->mpiComm, &status);
		assert(buf == groupi->mpiTag);
	}

	if (groupi->acquireFunc != NULL)
		rtn = groupi->acquireFunc(groupi->rankInGroup,
		                          groupi->sizeOfGroup,
		                          groupi->acquireFuncData);

	groupi->isAcquired = true;

	return rtn;
}

extern void
groupi_release(groupi_t groupi)
{
	assert(groupi != NULL);

	if (groupi->nextProcess >= 0) {
		MPI_Send(&(groupi->mpiTag), 1, MPI_INT, groupi->nextProcess,
		         groupi->mpiTag, groupi->mpiComm);
	}

	if (groupi->releaseFunc != NULL)
		groupi->releaseFunc(groupi->rankInGroup,
		                    groupi->sizeOfGroup,
		                    groupi->releaseFuncData);

	groupi->isAcquired = false;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_calcGroupDistribCommon(groupi_t groupi,
                             int      *rank,
                             int      *size,
                             int      *numLargeGroups)
{
	MPI_Comm_size(groupi->mpiComm, size);
	MPI_Comm_rank(groupi->mpiComm, rank);

	groupi->sizeOfGroup = *size / groupi->numGroups;
	*numLargeGroups     = *size % groupi->numGroups;

	groupi->prevProcess = -1;
	groupi->nextProcess = -1;
}

static void
local_calcGroupiDistribBlock(groupi_t groupi)
{
	int rank, size;
	int mid, numLargeGroups;

	local_calcGroupDistribCommon(groupi, &rank, &size, &numLargeGroups);
	mid = numLargeGroups * (groupi->sizeOfGroup + 1);

	if (rank < mid) {
		groupi->sizeOfGroup++;
		groupi->groupNumber = rank / (groupi->sizeOfGroup);
		groupi->rankInGroup = rank % (groupi->sizeOfGroup);
	} else {
		groupi->groupNumber = numLargeGroups
		                      + (rank - mid) / groupi->sizeOfGroup;
		groupi->rankInGroup = (rank - mid) % groupi->sizeOfGroup;
	}
	if (groupi->rankInGroup < groupi->sizeOfGroup - 1)
		groupi->nextProcess = rank + 1;
	if (groupi->rankInGroup > 0)
		groupi->prevProcess = rank - 1;
}

static void
local_calcGroupiDistribRoundRobin(groupi_t groupi)
{
	int rank, size;
	int numLargeGroups;

	local_calcGroupDistribCommon(groupi, &rank, &size, &numLargeGroups);

	groupi->groupNumber = rank % groupi->numGroups;
	groupi->rankInGroup = rank / groupi->numGroups;
	if (groupi->groupNumber < numLargeGroups)
		groupi->sizeOfGroup++;

	if (groupi->rankInGroup < groupi->sizeOfGroup - 1) {
		groupi->nextProcess = rank + groupi->numGroups;
		assert(groupi->nextProcess < size);
	}
	if (groupi->rankInGroup > 0) {
		groupi->prevProcess = rank - groupi->numGroups;
		assert(groupi->prevProcess >= 0);
	}
}
