// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/byteswap.c
 * @ingroup libutilMisc
 * @brief  This file provides the implementation of a function for
 *         byte-swapping.
 */


/*--- Includes ----------------------------------------------------------*/
#include "byteswap.h"


/*--- Implemenations of exported functions ------------------------------*/
extern void
byteswap(void *p, size_t s)
{
	int           n;
	unsigned char ptmp, *pc;

	pc = (unsigned char *)p;

	for (n = 0; n < s / 2; n++) {
		ptmp          = pc[n];
		pc[n]         = pc[s - n - 1];
		pc[s - n - 1] = ptmp;
	}

	return;
}
