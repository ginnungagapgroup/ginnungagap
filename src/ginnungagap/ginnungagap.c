// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagap.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagap_struct {
	uint32_t dim;
	int      seed;
};


/*--- Prototypes of local functions -------------------------------------*/
static void
local_parseIni(ginnungagap_t ginnungagap, parse_ini_t ini);


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini, int flags)
{
	ginnungagap_t ginnungagap;

	assert(ini != NULL);
	assert(flags == 0);

	ginnungagap = xmalloc(sizeof(struct ginnungagap_struct));
	local_parseIni(ginnungagap, ini);

	return ginnungagap;
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	assert(ginnungagap != NULL);

	printf("dim = %" PRIu32 "\n", ginnungagap->dim);
	printf("seed = %i\n", ginnungagap->seed);
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	xfree(*ginnungagap);
	*ginnungagap = NULL;
}

/*--- Defines for the Ini structure -------------------------------------*/
#define SECTION_NAME_MAIN "General"
#define KEY_NAME_DIM      "dim"
#define KEY_NAME_SEED     "seed"
#define getFromIni(func, ini, keyName, sectionName, trgt)      \
    if (!func(ini, keyName, sectionName, trgt)) {              \
		fprintf(stderr,                                        \
		        "FATAL:  Could not get %s from section %s.\n", \
		        keyName, sectionName);                         \
		exit(EXIT_FAILURE);                                    \
	}


/*--- Implementations of local functions --------------------------------*/
static void
local_parseIni(ginnungagap_t ginnungagap, parse_ini_t ini)
{
	uint32_t tmp;
	getFromIni(parse_ini_get_uint32, ini,
	           KEY_NAME_DIM, SECTION_NAME_MAIN,
	           &(ginnungagap->dim));
	getFromIni(parse_ini_get_uint32, ini,
	           KEY_NAME_SEED, SECTION_NAME_MAIN,
	           &tmp);
	ginnungagap->seed = (int)tmp;
}
