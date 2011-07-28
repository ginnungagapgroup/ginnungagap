// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/groupi.c
 * @ingroup libutilParallelGroupi
 * @brief  This file provides the implementation of the group parallel
 *         interface.
 */


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

/** @brief  Indicates no process. */
#define LOCAL_NOPROCESS -1


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Calculates the common number for the distribution.
 *
 * @param[in]   groupi
 *                 The group interface to work on.
 * @param[out]  *rank
 *                 The rank of this process.
 * @param[out]  *size
 *                 The size of the communicator.
 * @param[out]  *numLargeGroups
 *                 The number of groups that have to have an extra task.
 *
 * @return  Returns nothing.
 */
static void
local_calcGroupDistribCommon(const groupi_t groupi,
                             int            *rank,
                             int            *size,
                             int            *numLargeGroups);


/**
 * @brief  Calculates the distribution of the tasks in a block mode.
 *
 * @param[in,out]  groupi
 *                    The group interface to work with.
 *
 * @return  Returns nothing.
 */
static void
local_calcGroupiDistribBlock(groupi_t groupi);


/**
 * @brief  Calculates the distribution of the tasks in a round-robin mode.
 *
 * @param[in,out]  groupi
 *                    The group interface to work with.
 *
 * @return  Returns nothing.
 */
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
	assert(releaseFunc != NULL);

	groupi->releaseFunc     = releaseFunc;
	groupi->releaseFuncData = releaseFuncData;
}

extern int
groupi_getNumGroups(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->numGroups;
}

extern MPI_Comm
groupi_getMpiCommunicator(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->mpiComm;
}

extern int
groupi_getMpiTag(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->mpiTag;
}

extern int
groupi_getGroupNumber(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->groupNumber;
}

extern int
groupi_getSizeOfGroup(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->sizeOfGroup;
}

extern int
groupi_getRankInGroup(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->rankInGroup;
}

extern int
groupi_getPreviousProcess(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->prevProcess;
}

extern int
groupi_getNextProcess(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->nextProcess;
}

extern bool
groupi_isAcquired(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi->isAcquired ? true : false;
}

extern bool
groupi_isFirstInGroup(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi_getPreviousProcess(groupi) == LOCAL_NOPROCESS
	       ? true : false;
}

extern bool
groupi_isLastInGroup(const groupi_t groupi)
{
	assert(groupi != NULL);

	return groupi_getNextProcess(groupi) == LOCAL_NOPROCESS ? true : false;
}

extern void *
groupi_acquire(groupi_t groupi)
{
	void *rtn = NULL;

	assert(groupi != NULL);

	if (!groupi_isFirstInGroup(groupi)) {
		int        buf;
		MPI_Status status;

		MPI_Recv(&buf, 1, MPI_INT, groupi->prevProcess,
		         groupi->mpiTag, groupi->mpiComm, &status);
		assert(buf == groupi->mpiTag);
	}

	if (groupi->acquireFunc != NULL)
		rtn = groupi->acquireFunc(groupi->groupNumber,
		                          groupi->rankInGroup,
		                          groupi->sizeOfGroup,
		                          groupi->acquireFuncData);

	groupi->isAcquired = true;

	return rtn;
}

extern void
groupi_release(groupi_t groupi)
{
	assert(groupi != NULL);

	if (!groupi_isLastInGroup(groupi)) {
		MPI_Send(&(groupi->mpiTag), 1, MPI_INT, groupi->nextProcess,
		         groupi->mpiTag, groupi->mpiComm);
	}

	if (groupi->releaseFunc != NULL)
		groupi->releaseFunc(groupi->groupNumber,
		                    groupi->rankInGroup,
		                    groupi->sizeOfGroup,
		                    groupi->releaseFuncData);

	groupi->isAcquired = false;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_calcGroupDistribCommon(const groupi_t groupi,
                             int            *rank,
                             int            *size,
                             int            *numLargeGroups)
{
	MPI_Comm_size(groupi->mpiComm, size);
	MPI_Comm_rank(groupi->mpiComm, rank);

	groupi->sizeOfGroup = *size / groupi->numGroups;
	*numLargeGroups     = *size % groupi->numGroups;

	groupi->prevProcess = LOCAL_NOPROCESS;
	groupi->nextProcess = LOCAL_NOPROCESS;
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
