// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pFieldID.c
 * @ingroup  libg9pFieldID
 * @brief  Provides the implementation of the field ID.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pFieldID.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Implementations of exported functions -----------------------------*/
extern const char *
g9pFieldID_getStr(const g9pFieldID_t fid)
{
	static char *names[5] = {"wn", "delta", "velx", "vely", "velz"};
	assert(fid >= 0 && fid < 5);
	return names[fid];
}

/*--- Implementations of local functions --------------------------------*/
