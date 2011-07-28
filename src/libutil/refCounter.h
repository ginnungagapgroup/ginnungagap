// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef REFCOUNTER_H
#define REFCOUNTER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/refCounter.h
 * @ingroup libutilMisc
 * @brief  This file provides the interface of the reference counter.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Exported types ----------------------------------------------------*/

/** @brief  The reference counter type. */
typedef int refCounter_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Initializes a refence counter.
 *
 * @param[in,out]  *refCount
 *                    The reference counter to initialize.
 *
 * @return  Returns nothing.
 */
extern void
refCounter_init(refCounter_t *refCount);

/**
 * @brief  Creates a new reference (increases the reference counter).
 *
 * @param[in,out]  *curRef
 *                    The reference counter to work with.
 *
 * @return  Returns nothing.
 */
extern void
refCounter_ref(refCounter_t *curRef);

/**
 * @brief  Deletes a reference (decreases the reference counter).
 *
 * @param[in, out]  *curRef
 *                     The reference counter to work with.  The requires
 *                     that there still exists at least one reference.
 *
 * @return  Returns @c false if after dereferencing there are still
 *          references left and @c true, if the last reference has just
 *          been removed.
 */
extern bool
refCounter_deref(refCounter_t *curRef);

/**
 * @brief  Checks whether there are references left.
 *
 * @param[in]  curRef
 *                The reference conunter to check.
 *
 * @return  Returns @c true if no references are left and @c false
 *          otherwise.
 */
extern bool
refCounter_noReferenceLeft(refCounter_t curRef);


#endif
