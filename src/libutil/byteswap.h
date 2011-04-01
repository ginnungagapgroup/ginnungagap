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


#endif
