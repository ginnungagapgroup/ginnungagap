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
#include <string.h>
#include <stdbool.h>
#include "../libutil/xmem.h"
#ifdef WITH_SILO
#  include <silo.h>
#endif


/*--- Prototypes of local functions -------------------------------------*/
#ifdef WITH_MPI
static void
local_parseMPIStuff(ginnungagapConfig_t config, parse_ini_t ini);

#endif

#ifdef WITH_SILO
static void
local_parseSiloStuff(ginnungagapConfig_t config, parse_ini_t ini);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagapConfig_t
ginnungagapConfig_new(parse_ini_t ini)
{
	ginnungagapConfig_t config;

	assert(ini != NULL);

	config = xmalloc(sizeof(struct ginnungagapConfig_struct));
	getFromIni(&(config->dim1D), parse_ini_get_uint32,
	           ini, "dim1D", "Ginnungagap");
	getFromIni(&(config->boxsizeInMpch), parse_ini_get_double,
	           ini, "boxsizeInMpch", "Ginnungagap");
	getFromIni(&(config->gridName), parse_ini_get_string,
	           ini, "gridName", "Ginnungagap");
	getFromIni(&(config->useConstraints), parse_ini_get_bool,
	           ini, "useConstraints", "Ginnungagap");
	if (config->useConstraints) {
		getFromIni(&(config->fileNameConstraints), parse_ini_get_string,
		           ini, "fileNameConstraints", "Ginnungagap");
		getFromIni(&(config->dim1DConstraints), parse_ini_get_uint32,
		           ini, "dim1DConstraints", "Ginnungagap");
	} else {
		config->fileNameConstraints = NULL;
		config->dim1DConstraints    = UINT32_C(0);
	}
#ifdef WITH_MPI
	local_parseMPIStuff(config, ini);
#endif
#ifdef WITH_SILO
	local_parseSiloStuff(config, ini);
#endif

	return config;
}

extern void
ginnungagapConfig_del(ginnungagapConfig_t *config)
{
	assert(config != NULL && *config != NULL);

	if ((*config)->fileNameConstraints != NULL)
		xfree((*config)->fileNameConstraints);
	xfree((*config)->gridName);
#ifdef WITH_SILO
	xfree((*config)->filePrefix);
#endif

	xfree(*config);
	*config = NULL;
}

/*--- Implementations of local functions --------------------------------*/
#ifdef WITH_MPI
static void
local_parseMPIStuff(ginnungagapConfig_t config, parse_ini_t ini)
{
	int32_t *nProcs;
	bool    rtn;

	rtn = parse_ini_get_int32list(ini, "nProcs", "MPI", NDIM, &nProcs);
	if (!rtn) {
		fprintf(stderr, "Could not get nProcs from section MPI.\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < NDIM; i++)
		config->nProcs[i] = (int)(nProcs[i]);
	xfree(nProcs);
}

#endif

#ifdef WITH_SILO
static void
local_parseSiloStuff(ginnungagapConfig_t config, parse_ini_t ini)
{
	char     *dbtype;
	uint32_t numFiles;

	getFromIni(&(config->filePrefix), parse_ini_get_string,
	           ini, "filePrefix", "Silo");
	getFromIni(&(dbtype), parse_ini_get_string,
	           ini, "dbType", "Silo");
	if (strcmp(dbtype, "DB_HDF5") == 0) {
		config->dbtype = DB_HDF5;
	} else if (strcmp(dbtype, "DB_PDB") == 0) {
		config->dbtype = DB_PDB;
	} else {
		exit(EXIT_FAILURE);
	}
	xfree(dbtype);
#  ifdef WITH_MPI
	getFromIni(&numFiles, parse_ini_get_uint32, ini, "numFiles", "Silo");
	config->numFiles = (int)numFiles;
#  endif
}

#endif
