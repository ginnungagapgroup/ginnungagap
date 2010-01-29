// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagap.h"
#include "ginnungagapConfig.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagap_struct {
	ginnungagapConfig_t config;
};


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini, int flags)
{
	ginnungagap_t ginnungagap;
	assert(ini != NULL);
	assert(flags == 0);

	ginnungagap         = xmalloc(sizeof(struct ginnungagap_struct));
	ginnungagap->config = ginnungagapConfig_new(ini);

	return ginnungagap;
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	assert(ginnungagap != NULL);
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	ginnungagapConfig_del(&((*ginnungagap)->config));
	xfree(*ginnungagap);
	*ginnungagap = NULL;
}

/*--- Implementations of local functions --------------------------------*/
