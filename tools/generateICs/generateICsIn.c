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
generateICsIn_new(gridReader_t velx,  gridReader_t vely,  gridReader_t velz,
                  gridReader_t velx2, gridReader_t vely2, gridReader_t velz2)
{
	generateICsIn_t in;

	in          = xmalloc( sizeof(struct generateICsIn_struct) );
	in->velx    = velx;
	in->vely    = vely;
	in->velz    = velz;
	in->velx2   = velx2;
	in->vely2   = vely2;
	in->velz2   = velz2;

	in->varVelx  = dataVar_new("velx",  DATAVARTYPE_FPV, 1);
	in->varVely  = dataVar_new("vely",  DATAVARTYPE_FPV, 1);
	in->varVelz  = dataVar_new("velz",  DATAVARTYPE_FPV, 1);
	in->varVelx2 = (velx2 != NULL) ? dataVar_new("velx", DATAVARTYPE_FPV, 1) : NULL;
	in->varVely2 = (vely2 != NULL) ? dataVar_new("vely", DATAVARTYPE_FPV, 1) : NULL;
	in->varVelz2 = (velz2 != NULL) ? dataVar_new("velz", DATAVARTYPE_FPV, 1) : NULL;

	return in;
}

extern void
generateICsIn_del(generateICsIn_t *in)
{
	assert(in != NULL && *in != NULL);

	dataVar_del( &( (*in)->varVelx ) );
	dataVar_del( &( (*in)->varVely ) );
	dataVar_del( &( (*in)->varVelz ) );
	if ( (*in)->varVelx2 != NULL ) dataVar_del( &( (*in)->varVelx2 ) );
	if ( (*in)->varVely2 != NULL ) dataVar_del( &( (*in)->varVely2 ) );
	if ( (*in)->varVelz2 != NULL ) dataVar_del( &( (*in)->varVelz2 ) );

	gridReader_del( &( (*in)->velx ) );
	gridReader_del( &( (*in)->vely ) );
	gridReader_del( &( (*in)->velz ) );
	if ( (*in)->velx2 != NULL ) gridReader_del( &( (*in)->velx2 ) );
	if ( (*in)->vely2 != NULL ) gridReader_del( &( (*in)->vely2 ) );
	if ( (*in)->velz2 != NULL ) gridReader_del( &( (*in)->velz2 ) );
	xfree(*in);

	*in = NULL;
}

/*--- Implementations of local functions --------------------------------*/
