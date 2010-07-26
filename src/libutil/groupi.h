// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GROUPI_H
#define GROUPI_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>
#include <mpi.h>


/*--- Enums -------------------------------------------------------------*/
typedef enum {
	GROUPI_MODE_BLOCK,
	GROUPI_MODE_ROUNDROBIN
} groupi_mode_t;

/*--- ADT handle --------------------------------------------------------*/
typedef struct groupi_struct *groupi_t;


/*--- Callback types ----------------------------------------------------*/
typedef void * (*groupiAcquireFunc_t)(int seqNum, int seqLen, void *data);
typedef void (*groupiReleaseFunc_t)(int seqNum, int seqLen, void *data);


/*--- Prototypes of exported functions ----------------------------------*/
extern groupi_t
groupi_new(int           numGroups,
           MPI_Comm      mpiComm,
           int           mpiTag,
           groupi_mode_t mode);

extern void
groupi_del(groupi_t *groupi);

extern void
groupi_registerAcquireFunc(groupi_t            groupi,
                           groupiAcquireFunc_t acquireFunc,
                           void                *acquireFuncData);

extern void
groupi_registerReleaseFunc(groupi_t            groupi,
                           groupiReleaseFunc_t releaseFunc,
                           void                *releaseFuncData);


extern int
groupi_getRankInGroup(const groupi_t groupi);

extern int
groupi_getSizeOfGroup(const groupi_t groupi);

extern int
groupi_getNumGroups(const groupi_t groupi);

extern bool
groupi_isAcquired(const groupi_t groupi);

extern void *
groupi_acquire(groupi_t groupi);

extern void
groupi_release(groupi_t groupi);


#endif
