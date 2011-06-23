// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef VARARR_H
#define VARARR_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/varArr.h
 * @ingroup libutilMiscVarArr
 * @brief This file provides the interface the variable length arrays.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Exported defines --------------------------------------------------*/


/*--- ADT handle --------------------------------------------------------*/

/**
 * @brief  Defines the handle for a varArr object.
 */
typedef struct varArr_struct *varArr_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new variable length array with allocated space for
 *         a few elements.
 *
 * @param[in]  numElements
 *                The number of elements
 * @return  Returns a handle to a variable length array.
 */
extern varArr_t
varArr_new(int numElements);


/**
 * @brief  Deletes a variable length array and free all associated
 *         memory.
 *
 * Note that the object can only be deleted if the array is empty, i.e.
 * if all elements have been removed.  This is because the object does
 * not know how to delete the stored references (as it only stores
 * pointers to them and is oblivious to their types).
 *
 * @param[in,out]  *arr
 *                    A pointer to the variable holding the handle of
 *                    the variable length array that is to be deleted.
 *                    After deletion the external variable is set to
 *                    NULL.
 *
 * @return  Returns nothing.
 */
extern void
varArr_del(varArr_t *arr);


/** @} */

/**
 * @name Getter
 *
 * @{
 */

/**
 * @brief  Get the length of the array.
 *
 * @param[in] arr
 *               The array to query.
 *
 * @return  Returns the length of the array (i.e. the number of elements
 *          in the array).
 */
extern int
varArr_getLength(const varArr_t arr);


/**
 * @brief Gets the handle of a specific element.
 *
 * @param[in]  arr
 *                The array to query.
 * @param[in]  numElement
 *                The number of the element to retrieve.
 *
 * @return Returns the handle of the specified element.
 */
extern void *
varArr_getElementHandle(const varArr_t arr, int numElement);


/** @} */

/**
 * @name Using
 *
 * @{
 */

/**
 * @brief  Inserts an element in the array.
 *
 * The array will be resized, if there is not enough space available to
 * add the element.
 *
 * @param[in,out]  arr
 *                    The array into which to insert.
 * @param[in]      *element
 *                    The element to insert.
 *
 * @return Returns the position at which the element was inserted.
 */
extern int
varArr_insert(varArr_t arr, void *element);


/**
 * @brief  Removes an element from the array.
 *
 * If the array shrinks below a certain threshold, the array is resized
 * as to not waste too much memory.
 *
 * @param[in,out]  arr
 *                    The array from which to remove an element.
 * @param[in]      numElement
 *                    The number of the element to remove.
 *
 * @return  Returns the removed element.
 */
extern void *
varArr_remove(varArr_t arr, int numElement);


/**
 * @brief  Replaces an element in the array.
 *
 * @param[in,out]  arr
 *                    The array on which to perform the replacing.
 * @param[in]      numElement
 *                    The number of the element to replace.
 * @param[in]      *newElement
 *                    The new element.
 *
 * @return  Returns the old element.
 */
extern void *
varArr_replace(varArr_t arr, int numElement, void *newElement);


/**
 * @brief  This will truncate the array to zero size.
 *
 * All elements are simply discarded, be careful when using this
 * function, you might easily loose access to memory regions if you do
 * not retrieve the objects that are stored in the array first.
 *
 * @param[in,out]  arr
 *                    The array that should be truncated.
 */
extern void
varArr_truncate(varArr_t arr);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilMiscVarArr Variable length array
 * @ingroup libutilMisc
 * @brief Provides a variable length array.
 */


#endif
