// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagapConfig.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/xmem.h"


/*--- Defines for the Ini structure -------------------------------------*/
#define SECTION_NAME_MAIN "General"
#define KEY_NAME_DIM      "dim"
#define KEY_NAME_SEED     "seed"
#define getFromIni(trgt, func, ini, keyName, sectionName)      \
    if (!func(ini, keyName, sectionName, trgt)) {              \
		fprintf(stderr,                                        \
		        "FATAL:  Could not get %s from section %s.\n", \
		        keyName, sectionName);                         \
		exit(EXIT_FAILURE);                                    \
	}


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagapConfig_struct {
	uint32_t dim1D;
	int      randomSeed;
};


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagapConfig_t
ginnungagapConfig_new(parse_ini_t ini)
{
	ginnungagapConfig_t config;
	union {
		uint32_t u32;
		int32_t  i32;
	} tmp;
	assert(ini != NULL);

	config = xmalloc(sizeof(struct ginnungagapConfig_struct));
	getFromIni(&(tmp.u32), parse_ini_get_uint32,
	           ini, KEY_NAME_DIM, SECTION_NAME_MAIN);
	config->dim1D = tmp.u32;
	getFromIni(&(tmp.i32), parse_ini_get_int32,
	           ini, KEY_NAME_SEED, SECTION_NAME_MAIN);
	config->randomSeed = (int)(tmp.i32);

	return config;
}

extern void
ginnungagapConfig_del(ginnungagapConfig_t *config)
{
	assert(config != NULL);
	assert(*config != NULL);

	xfree(*config);
	*config = NULL;
}

/*--- Implementations of local functions --------------------------------*/
