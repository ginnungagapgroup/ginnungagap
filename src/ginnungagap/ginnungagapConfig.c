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
#include <stdbool.h>
#include "../libutil/xmem.h"


/*--- Defines for the Ini structure -------------------------------------*/
#define CONFIG_SECTION_NAME   "General"
#define CONFIG_HIGHDIM1D_NAME "highDim1D"
#define CONFIG_REPLACEMODES_NAME "replaceLowFreqModes"
#define CONFIG_LOWDIM1D_NAME  "lowDim1D"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagapConfig_t
ginnungagapConfig_new(parse_ini_t ini)
{
	ginnungagapConfig_t config;
	union {
		uint32_t u32;
		int32_t  i32;
		bool     bl;
	} tmp;
	assert(ini != NULL);

	config = xmalloc(sizeof(struct ginnungagapConfig_struct));
	getFromIni(&(tmp.u32), parse_ini_get_uint32,
	           ini, CONFIG_HIGHDIM1D_NAME, CONFIG_SECTION_NAME);
	config->highDim1D = tmp.u32;
	getFromIni(&(tmp.bl), parse_ini_get_bool,
	           ini, CONFIG_REPLACEMODES_NAME, CONFIG_SECTION_NAME);
	config->replaceLowFreqModes = tmp.bl;
	if (config->replaceLowFreqModes) {
		getFromIni(&(tmp.u32), parse_ini_get_uint32,
		           ini, CONFIG_LOWDIM1D_NAME, CONFIG_SECTION_NAME);
		config->lowDim1D = tmp.u32;
	} else {
		config->lowDim1D = UINT32_C(0);
	}

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
