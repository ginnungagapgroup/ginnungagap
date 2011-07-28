// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GROUPI_ADT_H
#define GROUPI_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/groupi_adt.h
 * @ingroup libutilParallelGroupi
 * @brief  This file provides the main structure of the group interface.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>
#include <mpi.h>


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure of the group interace. */
struct groupi_struct {
	/** @brief  The number of independent groups. */
	int                 numGroups;
	/** @brief  The MPI communicator to work with. */
	MPI_Comm            mpiComm;
	/** @brief  The MPI tag to use in groupi communications. */
	int                 mpiTag;
	/** @brief  The number of this group. */
	int                 groupNumber;
	/** @brief  The size of this group. */
	int                 sizeOfGroup;
	/** @brief  The rank of this task in this group. */
	int                 rankInGroup;
	/** @brief  The number of the previous task in this group. */
	int                 prevProcess;
	/** @brief  The number of the next task in this group. */
	int                 nextProcess;
	/** @brief  A flag indicating whether a lock has been obtained. */
	bool                isAcquired;
	/** @brief  The call-back function to call when a lock is been obtained.*/
	groupiAcquireFunc_t acquireFunc;
	/** @brief  The data to pass to the acquire call-back function. */
	void                *acquireFuncData;
	/** @brief  The call-back function to call when releasing a lock. */
	groupiReleaseFunc_t releaseFunc;
	/** @brief  The data to pass to the release call-back function. */
	void                *releaseFuncData;
};


#endif
