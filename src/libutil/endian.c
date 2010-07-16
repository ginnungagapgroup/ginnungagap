// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "endian.h"
#include <assert.h>
#include <stdbool.h>


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
endian_systemIsLittle(void)
{
	return endian_systemIsBig() ? false : true;
}

extern bool
endian_systemIsBig(void)
{
	const static int temp = 1;
	unsigned char    *p   = (unsigned char *)&temp;

	// This verifies that the in-memory representation of 1 is
	// 00 00 00 01 (which is big endian)
	// and not
	// 01 00 00 00 (which would be little endian)

	// Otherwise the magic might not work as intended
	assert(4 * sizeof(unsigned char) == sizeof(int));

	return ((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] == 1))
	       ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
