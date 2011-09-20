// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagapSetup.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Retrieves the normalisation mode from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file to read from.
 *
 * @return  Returns the mode.
 */
static g9pNorm_mode_t
local_getNormModeFromIni(parse_ini_t ini);


#ifdef WITH_MPI

/**
 * @brief  Helper function to parse the MPI setup.
 *
 * @param[in,out]  setup
 *                    The setup structure to fill.
 * @patam[in,out]  ini
 *                    The ini file to use.
 *
 * @return  Returns nothing.
 */
static void
local_parseMPIStuff(ginnungagapSetup_t setup, parse_ini_t ini);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagapSetup_t
ginnungagapSetup_new(parse_ini_t ini)
{
	ginnungagapSetup_t setup;

	assert(ini != NULL);

	setup = xmalloc(sizeof(struct ginnungagapSetup_struct));
	getFromIni(&(setup->dim1D), parse_ini_get_uint32,
	           ini, "dim1D", "Ginnungagap");
	getFromIni(&(setup->boxsizeInMpch), parse_ini_get_double,
	           ini, "boxsizeInMpch", "Ginnungagap");
	getFromIni(&(setup->zInit), parse_ini_get_double,
	           ini, "zInit", "Ginnungagap");
	getFromIni(&(setup->gridName), parse_ini_get_string,
	           ini, "gridName", "Ginnungagap");
	setup->normalisationMode = local_getNormModeFromIni(ini);
#ifdef WITH_MPI
	local_parseMPIStuff(setup, ini);
#endif

	return setup;
}

extern void
ginnungagapSetup_del(ginnungagapSetup_t *setup)
{
	assert(setup != NULL && *setup != NULL);

	xfree((*setup)->gridName);

	xfree(*setup);
	*setup = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static g9pNorm_mode_t
local_getNormModeFromIni(parse_ini_t ini)
{
	char           *name;
	g9pNorm_mode_t mode;

	getFromIni(&name, parse_ini_get_string,
	           ini, "normalisationMode", "Ginnungagap");
	mode = g9pNorm_getModeFromName(name);
	if (mode == G9PNORM_MODE_UNKNOWN) {
		fprintf(stderr, "Normalisation mode %s unknown\n", name);
		diediedie(EXIT_FAILURE);
	}

	xfree(name);

	return mode;
}

#ifdef WITH_MPI
static void
local_parseMPIStuff(ginnungagapSetup_t setup, parse_ini_t ini)
{
	int32_t *nProcs;
	bool    rtn;

	rtn = parse_ini_get_int32list(ini, "nProcs", "MPI", NDIM, &nProcs);
	if (!rtn) {
		fprintf(stderr, "Could not get nProcs from section MPI.\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < NDIM; i++)
		setup->nProcs[i] = (int)(nProcs[i]);
	xfree(nProcs);
}

#endif
