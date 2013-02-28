// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsIn.c
 * @ingroup  toolsGICSIn
 * @brief  Implements wrapper for input things.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICsIn.h"
#include <assert.h>
#include "../../src/libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern generateICsIn_t
generateICsIn_new(gridReader_t velx, gridReader_t vely, gridReader_t velz)
{
	generateICsIn_t in;

	in       = xmalloc( sizeof(struct generateICsIn_struct) );
	in->velx = velx;
	in->vely = vely;
	in->velz = velz;

	return in;
}

extern void
generateICsIn_del(generateICsIn_t *in)
{
	assert(in != NULL && *in != NULL);

	gridReader_del( &( (*in)->velx ) );
	gridReader_del( &( (*in)->vely ) );
	gridReader_del( &( (*in)->velz ) );
	xfree(*in);

	*in = NULL;
}

/*--- Implementations of local functions --------------------------------*/
