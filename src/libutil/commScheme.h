// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEME_H
#define COMMSCHEME_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commScheme.h
 * @ingroup libutilParallelCommScheme
 * @brief  Provides a communication scheme interface (only available
 *         with MPI).
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "commSchemeBuffer.h"


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a communication scheme object.
 */
typedef struct commScheme_struct *commScheme_t;


/*--- Exported defines --------------------------------------------------*/

/** @brief Indicates sending. */
#define COMMSCHEME_TYPE_SEND 0
/** @brief Indicates receiving. */
#define COMMSCHEME_TYPE_RECV 1


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new communication scheme.
 *
 * @param[in]  comm
 *                The MPI communicator that is used in the communication
 *                scheme.  This must be a valid communicator
 *                (MPI_COMM_NULL is not allowed).
 * @param[in]  tag
 *                The tag that is used in the MPI communication calls.
 *                This needs to be in [0,32767[ (which is the range of
 *                tags allowed by MPI).
 *
 * @return  Returns a handle to a communication scheme.
 */
extern commScheme_t
commScheme_new(MPI_Comm comm, int tag);


/**
 * @brief  Deletes a communication scheme object and frees all memory.
 *
 * If the communication scheme is still doing communications, the
 * function will wait for them to finish before continuing with the
 * deletion process.
 *
 * @param[in,out]  *scheme
 *                    Pointer to the variable that holds the handle of
 *                    the communication scheme.  Note that after
 *                    deletion this will be set to NULL.
 *
 * @return  Returns nothing.
 */
extern void
commScheme_del(commScheme_t *scheme);


/** @} */

/**
 * @name Setting up
 *
 * @{
 */

/**
 * @brief  This adds a new buffer to a given scheme.
 *
 * This function can only be used before the communication scheme is
 * executed with commScheme_fire().  After the buffer has been added,
 * the communication scheme will take care of freeing the memory used
 * for the buffer object, so the calling function must not free the
 * buffer object by itself.
 *
 * @param[in,out]  scheme
 *                    The scheme to which the buffer should be added.
 *                    Must not be NULL.
 * @param[in]      buffer
 *                    The buffer to add.  Must not be NULL.
 * @param[in]      type
 *                    Describes whether the buffer is used for sending
 *                    or receiving, this must be either
 *                    #COMMSCHEME_TYPE_SEND or #COMMSCHEME_TYPE_RECV.
 *
 * @return  Returns the position in the (send or receive) array at which
 *          the has been added.
 */
extern int
commScheme_addBuffer(commScheme_t       scheme,
                     commSchemeBuffer_t buffer,
                     int                type);


/** @} */

/**
 * @name Using
 *
 * @{
 */

/**
 * @brief  This will execute the communication in a non-blocking way.
 *
 * This function will first start non-blocking receive operations
 * according to the previously added receive, with
 * commScheme_addBuffer(), buffers (starting from the first one that has
 * been added proceeding in order to the last one).  Then non-blocking
 * sends will be issued, but the buffers will be inspected, starting
 * from the first added one, if they are to be sent to a process with a
 * higher rank than the sending one.  As soon as the first such buffer
 * is found, sending proceeds in the order of the buffers.  If no buffer
 * is send to a higher rank, sending commences with the first one.
 *
 * This method is meant to prevent congestion:  If the buffers are added
 * in order of the ranks they are send to (likely), than all processes
 * would send in the same way, e.g. first all are sending to the first
 * process, then all are sending to the second and so on.  With the
 * method employed here, every process sends in steps to the right, e.g.
 * rank 0 sends to rank 1, rank 1 to rank 2, then rank 0 to rank 2 and
 * rank 1 to rank 0.  This way the network can be used more efficiently.
 *
 * Note that even though all sends are non-blocking, there is no
 * guarantee that the all sends are executed at the same time by the
 * library.  It is much more likely that the sends are processed in the
 * order they arrive, though this will depend on the MPI library.
 *
 * @param[in,out] scheme
 *                   The handle of the scheme that should be executed.
 *                   Must not be NULL and must not have been executed
 *                   before.
 *
 * @return  Returns nothing.
 */
extern void
commScheme_fire(commScheme_t scheme);


/**
 * @brief  This will execute the communication in a blocking way.
 *
 * This works like commScheme_fire(), except that the function will only
 * return after all communications have ended.  In essence, this
 * function is same as calling commScheme_fire() immediately followed by
 * commScheme_wait().
 *
 * @param[in,out] scheme
 *                   The handle of the scheme that should be executed.
 *                   Must not be NULL and must not have been executed
 *                   before.
 *
 * @return  Returns nothing.
 */
extern void
commScheme_fireBlock(commScheme_t scheme);


/**
 * @brief  This will wait for all communication to finish in
 *         communication scheme.
 *
 * @param[in,out]  scheme
 *                    The scheme for which to wait.  Must not be NULL
 *                    and commScheme_fire() must have been the last
 *                    function that has been called on the same handle.
 *
 * @return  Returns nothing.
 */
extern void
commScheme_wait(commScheme_t scheme);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilParallelCommScheme Communication Scheme
 * @ingroup libutilParallel
 * @brief Provides a communication scheme interface.
 */


#endif
