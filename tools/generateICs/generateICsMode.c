// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsMode.c
 * @ingroup  toolsGICSMode
 * @brief  Implements the mode object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICsMode.h"
#include <assert.h>
#include <string.h>
#include "../../src/libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern generateICsMode_t
generateICsMode_new(const bool doGas, const bool useLongIDs, const bool autoCenter)
{
	generateICsMode_t             mode;
	struct generateICsMode_struct tmp = {
		.doGas      = doGas,
		.useLongIDs = useLongIDs,
		.autoCenter = autoCenter
	};

	mode = xmalloc( sizeof(struct generateICsMode_struct) );
	memcpy( mode, &tmp, sizeof(struct generateICsMode_struct) );

	return mode;
}

extern void
generateICsMode_del(generateICsMode_t *mode)
{
	assert(mode != NULL && *mode != NULL);

	xfree(*mode);

	*mode = NULL;
}

/*--- Implementations of local functions --------------------------------*/
