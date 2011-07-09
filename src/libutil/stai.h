// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef STAI_H
#define STAI_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  stai.h
 * @ingroup  libutilMiscStai
 * @brief  This file provides the interface to stridden arrays (stridden
 *         array interface, stai).
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the stai object. */
typedef struct stai_struct *stai_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new stai object.
 *
 * @param[in]  *base
 *                The pointer to the beginning of the array (the first
 *                element).  Note that this becomes invalid as soon as
 *                the array is resized (reallocated) or deleted/freed.
 *                Passing @c NULL is undefined.
 * @param[in]  sizeOfElementInBytes
 *                The size of on element of the array in bytes.  This
 *                must be non-zero.
 * @param[in]  strideInBytes
 *                The distance between two elements in the array.  This
 *                must be at least as large as @c sizeOfElementInBytes.
 *
 * @return  Returns a new stai object for the provided array
 *          specifications.
 */
extern stai_t
stai_new(void         *base,
         unsigned int sizeOfElementInBytes,
         unsigned int strideInBytes);


/**
 * @brief  Creates a new stai object from a provided one with a
 *         different base.
 *
 * @param[in]  stai
 *                The stai object that should be used as a reference.
 *                Passing @c NULL is undefined.
 * @param[in]  *newBase
 *                The first element of the new array.  Passing @c NULL
 *                is undefined.
 *
 * @return  Returns a stai object that has the same size and stride
 *          properties as the provided stai object, but differs in the
 *          base object.
 */
extern stai_t
stai_cloneWithDifferentBase(const stai_t stai, void *newBase);


/**
 * @brief  Deletes a stai object and frees the associated memory.
 *
 * @param[in,out]  *stai
 *                    A pointer to an external variable holding the
 *                    reference to the stai object.  After deletion, the
 *                    external variable will be set to @c NULL, thus
 *                    removing access to an invalid memory region.
 *
 * @return  Returns nothing.
 */
extern void
stai_del(stai_t *stai);


/** @} */

/**
 * @name  Getter
 *
 * @{
 */

/**
 * @brief  Retrievs the base of the stai object.
 *
 * @param[in]  stai
 *                The stai object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the base of the stai.
 */
extern void *
stai_getBase(const stai_t stai);

/**
 * @brief  Retrievs the size of one element.
 *
 * @param[in]  stai
 *                The stai object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns size of one element in bytes.
 */
extern int
stai_getSizeOfElementInBytes(const stai_t stai);

/**
 * @brief  Retrievs the stride of the stai.
 *
 * @param[in]  stai
 *                The stai object to query.  Passing @c NULL is
 *                undefined.
 *
 * @return  Returns the stride of the stai in bytes.
 */
extern int
stai_getStrideInBytes(const stai_t stai);

/** @} */

/**
 * @name  Using
 *
 * @{
 */

/**
 * @brief  Sets an element at a given array position.
 *
 * @param[in]  stai
 *                The stai object to use.  Passing @c NULL is undefined.
 * @param[in]  pos
 *                The position in the array at which to set the element.
 *                It is the responsibility of the user to not exceed
 *                the allocated size of the array.
 * @param[in]  *element
 *                The element to add.  This must be at least of the
 *                size the stai assumes for elements.  Bytes exceeding
 *                the specified size of an object are ignored.  Passing
 *                @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
extern void
stai_setElement(const stai_t stai, uint64_t pos, const void *element);


/**
 * @brief  Sets multiple consecutive elements of an stai.
 *
 * This can be used to explode the values of a proper linear array into
 * a stridden version of the array (it will unpack an array).
 *
 * @param[in]  stai
 *                The stai object to use.  Passing @c NULL is valid iff
 *                @c numElements is @c 0.
 * @param[in]  pos
 *                The position in the stai from which to start setting
 *                the elements.  It is the responsibility of the caller
 *                to make sure that only allocated memory is accessed,
 *                i.e. that the stai is large enough to at least
 *                accommodate <tt>numElements + pos</tt> elements.
 * @param[in]  *elements
 *                Pointer to the first element.  This must be a linear
 *                array of at least @c numElements length and each
 *                element mus have exactly the size the stai is
 *                specified for.  Passing @c NULL is valid iff
 *                @c numElements is @c 0.
 * @param[in]  numElements
 *                The number of elements in the array pointed to be
 *                <tt>*elements</tt>.  If this is @c 0, the function
 *                does nothing.
 *
 * @return  Returns nothing.
 */
extern void
stai_setElementsMulti(const stai_t stai,
                      uint64_t     pos,
                      const void   *elements,
                      uint64_t     numElements);

/**
 * @brief  Gets an element form the stai.
 *
 * This is the exact inverse of stai_setElement(), it will copy the
 * value of an element in the stai to a memory region provided by the
 * caller.
 *
 * @param[in]   stai
 *                 The stai to use.  Passing @c NULL is undefined.
 * @param[in]   pos
 *                 The position of the element that should be retrieved.
 *                 The caller is responsible for ensuring that this is a
 *                 valid element.
 * @param[out]  *element
 *                 The memory region that will receive the value of the
 *                 element.  This must be large enough to actually
 *                 accommodate an element and passing @c NULL is
 *                 undefined.
 *
 * @return  Returns nothing.
 */
extern void
stai_getElement(const stai_t stai, uint64_t pos, void *element);

/**
 * @brief  Gets multiple consecutive elements from a stai.
 *
 * This is the exact inverse of stai_setElementsMulti(), it implodes
 * the stai array into a linear array for a given amount of elements
 * (it will pack).
 *
 * @param[in]   stai
 *                 The stai to use.  Passing @c NULL is allowed iff
 *                 @c numElements is @c 0.
 * @param[in]   pos
 *                 The position in the stai array from which to start
 *                 copying out.  The caller is responsible to ensure
 *                 that the stai contains at least
 *                  <tt>pos + numElements</tt> elements.
 * @param[out]  *elements
 *                 The memory region into which to copy the result.  The
 *                 caller has to ensure that this can hold
 *                 @c numElements elements of the size provided when the
 *                 stai was created.  It may be @c NULL iff
 *                 @c numElements is @c 0.
 * @param[in]   numElements
 *                 The number of elements to copy out.
 *
 * @return  Returns nothing.
 */
extern void
stai_getElementsMulti(const stai_t stai,
                      uint64_t     pos,
                      void         *elements,
                      uint64_t     numElements);

/**
 * @brief  This allows to move the base of the stai.
 *
 * @param[in,out]  stai
 *                    The stai to rebase.  Passing @c NULL is undefined.
 * @param[in]      offset
 *                    This is amount of elements the base should be
 *                    moved.  Positive values move the base to later
 *                    elements and negative values shift the base to
 *                    earlier values.  It the responsibility of the
 *                    caller to ensure that the base is only moved
 *                    within the array.  When an offset of 0 is
 *                    specified, the functions does nothing.
 *
 * @return  Returns nothing.
 */
extern void
stai_rebase(stai_t stai, int64_t offset);


/** @} */


/**
 * @name  Tests
 *
 * @{
 */

/**
 * @brief  This tests if the stai describes a linear array.
 *
 * A linear array is to be understood as an array whose elements are
 * consecutive in memory with no gaps between the elements, i.e. the
 * size of one element is equal to the stride value.
 *
 * @param[in]  stai
 *                The stai to test.  Passing @c NULL is undefined.
 *
 * @return  Returns true if the stai describes a linear array and false
 *          if not.
 */
extern bool
stai_isLinear(const stai_t stai);

/** @} */



/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutilMiscStai Stridden Array Interface (stai)
 * @ingroup libutilMisc
 * @brief Provides the interface for stridden arrays.
 */


#endif
