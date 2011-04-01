// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GROUPI_H
#define GROUPI_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/groupi.h
 * @ingroup libutilParallelGroupi
 * @brief  This file provides the group parallel interface.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>
#include <mpi.h>


/*--- Enums -------------------------------------------------------------*/

/**
 * @brief The mode in which the processes are assigned to groups.
 */
typedef enum {
	/** @brief A block of consecutive tasks is assign in one group. */
	GROUPI_MODE_BLOCK,
	/** @brief Consecutive tasks have consecutive group numbers. */
	GROUPI_MODE_ROUNDROBIN
} groupi_mode_t;


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for the group interface.
 */
typedef struct groupi_struct *groupi_t;


/*--- Callback types ----------------------------------------------------*/

/**
 * @brief The signature of the function that is called when the lock is
 *        acquired.
 */
typedef void * (*groupiAcquireFunc_t)(int seqID, int seqNum, int seqLen,
                                      void *data);

/**
 * @brief The signature of the function that is called when the lock is
 *        released.
 */
typedef void (*groupiReleaseFunc_t)(int seqID, int seqNum, int seqLen,
                                    void *data);


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new group interface.
 *
 * @param[in]  numGroups
 *                The number of groups to create.
 * @param[in]  mpiComm
 *                The MPI communicator to subdivide into groups.
 * @param[in]  mpiTag
 *                The MPI tag used in the communications.
 * @param[in]  mode
 *                The distribution mode of the task onto the groups.
 *
 * @return  Returns a new group interface.
 */
extern groupi_t
groupi_new(int           numGroups,
           MPI_Comm      mpiComm,
           int           mpiTag,
           groupi_mode_t mode);

extern void
groupi_del(groupi_t *groupi);


/** @} */

/**
 * @name Setup
 *
 * @{
 */


extern void
groupi_registerAcquireFunc(groupi_t            groupi,
                           groupiAcquireFunc_t acquireFunc,
                           void                *acquireFuncData);

extern void
groupi_registerReleaseFunc(groupi_t            groupi,
                           groupiReleaseFunc_t releaseFunc,
                           void                *releaseFuncData);


/** @} */

/**
 * @name Getter
 *
 * @{
 */


extern int
groupi_getNumGroups(const groupi_t groupi);

extern MPI_Comm
groupi_getMpiCommunicator(const groupi_t groupi);

extern int
groupi_getMpiTag(const groupi_t groupi);

extern int
groupi_getGroupNumber(const groupi_t groupi);

extern int
groupi_getSizeOfGroup(const groupi_t groupi);

extern int
groupi_getRankInGroup(const groupi_t groupi);

extern int
groupi_getPreviousProcess(const groupi_t groupi);

extern int
groupi_getNextProcess(const groupi_t groupi);


/** @} */

/**
 * @name Using
 *
 * @{
 */


extern bool
groupi_isAcquired(const groupi_t groupi);

extern bool
groupi_isFirstInGroup(const groupi_t groupi);

extern bool
groupi_isLastInGroup(const groupi_t groupi);

extern void *
groupi_acquire(groupi_t groupi);

extern void
groupi_release(groupi_t groupi);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilParallelGroupi Group Interface
 * @ingroup libutilParallel
 * @brief Provides a group based parallelization interface.
 */


#endif
