// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEMEBUFFER_H
#define COMMSCHEMEBUFFER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commSchemeBuffer.h
 * @ingroup libutilParallelCommScheme
 * @brief  Provides a communication buffer interface (only with MPI).
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <mpi.h>


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a communication buffer object.
 */
typedef struct commSchemeBuffer_struct *commSchemeBuffer_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new communication buffer.
 *
 * @param[in]  *buf
 *                Pointer to the memory region serving as the buffer
 *                space.
 * @param[in]  count
 *                The number of elements stored in the buffer.
 * @param[in]  datatype
 *                The (MPI) datatype of the elements.
 * @param[in]  rank
 *                The rank this buffer is for.
 *
 * @return  Returns a handle to a communication buffer.
 */
extern commSchemeBuffer_t
commSchemeBuffer_new(void *buf, int count, MPI_Datatype datatype, int rank);


/**
 * @brief  Deletes a communication buffer.
 *
 * @param[in,out]  *buf
 *                    A pointer to the handle of the buffer to be
 *                    deleted.  After the deletion the handle will be
 *                    set to NULL.
 *
 * @return  Returns nothing.
 */
extern void
commSchemeBuffer_del(commSchemeBuffer_t *buf);


/** @} */

/**
 * @name Using
 *
 * @{
 */

/**
 * @brief  Gets the handle of the actual buffer array in the buffer
 *         object.
 *
 * @param[in]  buf
 *                The buffer object to query.
 *
 * @return  Returns the pointer to the buffer array.  This is the same
 *          pointer as has been provided in commSchemeBuffer_new().
 */
extern void *
commSchemeBuffer_getBuf(const commSchemeBuffer_t buf);


/**
 * @brief  Gets the number of elements in the buffer.
 *
 * @param[in]  buf
 *                The buffer object to query.
 *
 * @return  Returns the number of elements.
 */
extern int
commSchemeBuffer_getCount(const commSchemeBuffer_t buf);


/**
 * @brief  Gets the MPI datatype of the elements in the buffer.
 *
 * @param[in]  buf
 *                The buffer object to query.
 *
 * @return  Returns the MPI datatype of the elements in the buffer.
 */
extern MPI_Datatype
commSchemeBuffer_getDatatype(const commSchemeBuffer_t buf);


/**
 * @brief  Gets the associated rank of the buffer.
 *
 * @param[in]  buf
 *                The buffer object to query.
 *
 * @return  Returns the associated rank.
 */
extern int
commSchemeBuffer_getRank(const commSchemeBuffer_t buf);


/** @} */

#endif
