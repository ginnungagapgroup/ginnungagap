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


/*--- Prototypes of local functions -------------------------------------*/
#ifdef WITH_MPI
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
	getFromIni(&(setup->useConstraints), parse_ini_get_bool,
	           ini, "useConstraints", "Ginnungagap");
	if (setup->useConstraints) {
		getFromIni(&(setup->fileNameConstraints), parse_ini_get_string,
		           ini, "fileNameConstraints", "Ginnungagap");
		getFromIni(&(setup->dim1DConstraints), parse_ini_get_uint32,
		           ini, "dim1DConstraints", "Ginnungagap");
	} else {
		setup->fileNameConstraints = NULL;
		setup->dim1DConstraints    = UINT32_C(0);
	}
	getFromIni(&(setup->forceSigma8InBox), parse_ini_get_bool,
	           ini, "forceSigma8InBox", "Ginnungagap");
#ifdef WITH_MPI
	local_parseMPIStuff(setup, ini);
#endif

	return setup;
} /* ginnungagapSetup_new */

extern void
ginnungagapSetup_del(ginnungagapSetup_t *setup)
{
	assert(setup != NULL && *setup != NULL);

	if ((*setup)->fileNameConstraints != NULL)
		xfree((*setup)->fileNameConstraints);
	xfree((*setup)->gridName);

	xfree(*setup);
	*setup = NULL;
}

/*--- Implementations of local functions --------------------------------*/
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
