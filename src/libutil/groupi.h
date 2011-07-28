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


/**
 * @brief  Deletes a group interface.
 *
 * @param[in,out]  *groupi
 *                     The group interface that should be deleted.
 *
 * @return  Returns nothing.
 */
extern void
groupi_del(groupi_t *groupi);


/** @} */

/**
 * @name Setup
 *
 * @{
 */

/**
 * @brief  Registers the call-back function called when aquiring.
 *
 * @param[in,out]  groupi
 *                    The group interface for which to register the
 *                    acquire call-back function.
 * @param[in]      acquireFunc
 *                    The call-back function.
 * @param[in]      *acquireFuncData
 *                    The data that should be passed to the call-back
 *                    function.
 *
 * @return  Returns nothing.
 */
extern void
groupi_registerAcquireFunc(groupi_t            groupi,
                           groupiAcquireFunc_t acquireFunc,
                           void                *acquireFuncData);


/**
 * @brief  Registers the call-back function called when releasing.
 *
 * @param[in,out]  groupi
 *                    The group interface for which to register the
 *                    release call-back function.
 * @param[in]      releaseFunc
 *                    The call-back function.
 * @param[in]      *releaseFuncData
 *                    The data that should be passed to the call-back
 *                    function.
 *
 * @return  Returns nothing.
 */
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

/**
 * @brief  Queries for the number of groups.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the number of groups in the group interface.
 */
extern int
groupi_getNumGroups(const groupi_t groupi);


/**
 * @brief  Queries for the MPI communicator used.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns MPI communicator used.
 */
extern MPI_Comm
groupi_getMpiCommunicator(const groupi_t groupi);


/**
 * @brief  Queries for the MPI tag that is used in communications.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the MPI tag used in communications within the group
 *          interface.
 */
extern int
groupi_getMpiTag(const groupi_t groupi);


/**
 * @brief  Queries for the group number.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the number of the group the group interface (on the
 *          current MPI task) belongs to.
 */
extern int
groupi_getGroupNumber(const groupi_t groupi);


/**
 * @brief  Queries for the size of the group.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the size of the group.
 */
extern int
groupi_getSizeOfGroup(const groupi_t groupi);


/**
 * @brief  Queries for rank in the group.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the rank of the calling process within the group
 *          interface group it belongs to.
 */
extern int
groupi_getRankInGroup(const groupi_t groupi);


/**
 * @brief  Queries for the previous process.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the rank of the process that acted before the
 *          process that is calling.
 */
extern int
groupi_getPreviousProcess(const groupi_t groupi);


/**
 * @brief  Queries for the next process.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the rank of the next process in the group.
 */
extern int
groupi_getNextProcess(const groupi_t groupi);


/** @} */

/**
 * @name Using
 *
 * @{
 */

/**
 * @brief  Checks whether the current process has acquired the right to
 *         work.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns @c true if the calling process has acquired the
 *          right to work and @c false otherwise.
 */
extern bool
groupi_isAcquired(const groupi_t groupi);


/**
 * @brief  Checks whether the calling process is the first one in the
 *         group.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns @c true if the calling process is the first process
 *          in the group and @c false if it is not.
 */
extern bool
groupi_isFirstInGroup(const groupi_t groupi);


/**
 * @brief  Checks whether the calling process is the last one in the
 *         group.
 *
 * @param[in]  groupi
 *                The group interface to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns @c true if the calling process is the last process
 *          in the group and @c false if it is not.
 */
extern bool
groupi_isLastInGroup(const groupi_t groupi);


/**
 * @brief  Will try to acquire the right to work (waits for all process
 *         which come first in the group to do their work).
 *
 * @param[in,out]  groupi
 *                    The group interface with which to work.
 *
 * @return  Returns the result of the acquire call-back function.
 */
extern void *
groupi_acquire(groupi_t groupi);


/**
 * @brief  Releases the group lock so that the next process in the group
 *         can work.
 *
 * @param[in,out]  groupi
 *                    The group interface with which to work.
 *
 * @return  Returns nothing.
 */
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
