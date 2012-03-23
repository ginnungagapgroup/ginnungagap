// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BYTESWAP_H
#define BYTESWAP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/byteswap.h
 * @ingroup libutilMisc
 * @brief  This file provides a function for byte-swapping.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdlib.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Performs a byteswapping.
 *
 * This will simply revers the order of the bytes, i.e. ABCD will
 * become DCBA.
 *
 * @param[in,out]  *p
 *                    The pointer to the variable that should be
 *                    byteswapped.
 * @param[in]      s
 *                    The number of bytes of the variable.
 *
 * @return  Returns nothing.
 */
extern void
byteswap(void *p, size_t s);

/**
 * @brief  Performs a bytswapping of the components of a vector.
 *
 * This will reverse the order of the bytes for each component of a vector
 * (linear array).  I.e. a vector with 3 2byte components ABCDEF will be
 * changed to BADCFE.
 *
 * @param[in,out]  *vec
 *                    The data that should be swapped.
 * @param[in]      sizeOfVec
 *                    The total size in bytes of the vector.
 * @param[in]      numComponents
 *                    The number of components in the vector.
 *
 * @return  Returns nothing.
 */
extern void
byteswapVec(void *vec, size_t sizeOfVec, int numComponents);

#endif
