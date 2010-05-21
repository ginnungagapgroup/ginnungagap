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
#ifdef WITH_SILO
#  include <silo.h>
#endif


/*--- Prototypes of local functions -------------------------------------*/
#ifdef WITH_MPI
static void
local_parseMPIStuff(ginnungagapSetup_t setup, parse_ini_t ini);

#endif

#ifdef WITH_SILO
static void
local_parseSiloStuff(ginnungagapSetup_t setup, parse_ini_t ini);

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
#ifdef WITH_MPI
	local_parseMPIStuff(setup, ini);
#endif
#ifdef WITH_SILO
	local_parseSiloStuff(setup, ini);
#endif

	return setup;
}

extern void
ginnungagapSetup_del(ginnungagapSetup_t *setup)
{
	assert(setup != NULL && *setup != NULL);

	if ((*setup)->fileNameConstraints != NULL)
		xfree((*setup)->fileNameConstraints);
	xfree((*setup)->gridName);
#ifdef WITH_SILO
	xfree((*setup)->filePrefix);
#endif

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

#ifdef WITH_SILO
static void
local_parseSiloStuff(ginnungagapSetup_t setup, parse_ini_t ini)
{
	char     *dbtype;
	uint32_t numFiles;

	getFromIni(&(setup->filePrefix), parse_ini_get_string,
	           ini, "filePrefix", "Silo");
	getFromIni(&(dbtype), parse_ini_get_string,
	           ini, "dbType", "Silo");
	if (strcmp(dbtype, "DB_HDF5") == 0) {
		setup->dbtype = DB_HDF5;
	} else if (strcmp(dbtype, "DB_PDB") == 0) {
		setup->dbtype = DB_PDB;
	} else {
		exit(EXIT_FAILURE);
	}
	xfree(dbtype);
#  ifdef WITH_MPI
	getFromIni(&numFiles, parse_ini_get_uint32, ini, "numFiles", "Silo");
	setup->numFiles = (int)numFiles;
#  endif
}

#endif
