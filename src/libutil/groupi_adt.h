// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GROUPI_ADT_H
#define GROUPI_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/groupi_adt.h
 * @ingroup libutilFilesParallel
 * @brief  This file provides the main structure of the group interface.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>
#include <mpi.h>


/*--- ADT implementation ------------------------------------------------*/
struct groupi_struct {
	int                 numGroups;
	MPI_Comm            mpiComm;
	int                 mpiTag;
	int                 groupNumber;
	int                 sizeOfGroup;
	int                 rankInGroup;
	int                 prevProcess;
	int                 nextProcess;
	bool                isAcquired;
	groupiAcquireFunc_t acquireFunc;
	void                *acquireFuncData;
	groupiReleaseFunc_t releaseFunc;
	void                *releaseFuncData;
};


#endif
