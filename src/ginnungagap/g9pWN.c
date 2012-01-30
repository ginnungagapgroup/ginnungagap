// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pWN.c
 * @ingroup  ginnungagapWN
 * @brief  Provides the implementation of the WN routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pWN.h"
#include <assert.h>
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridReader.h"
#include "../libgrid/gridWriter.h"
#include "../libgrid/gridWriterFactory.h"
#include "../libgrid/gridPatch.h"


/*--- Implemention of main structure ------------------------------------*/
#include "g9pWN_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_newGetInput(g9pWN_t     wn,
                  parse_ini_t ini,
                  const char  *sectionName);

static void
local_newGetOutput(g9pWN_t     wn,
                   parse_ini_t ini,
                   const char  *sectionName);

static void
local_setupFromRNG(g9pWN_t     wn,
                   gridPatch_t patch,
                   int         idxOfDensVar);


/*--- Implementations of exported functios ------------------------------*/
extern g9pWN_t
g9pWN_newFromIni(parse_ini_t ini, const char *sectionName)
{
	g9pWN_t wn;

	assert(ini != NULL);
	assert(sectionName != NULL);

	wn = xmalloc(sizeof(struct g9pWN_struct));
	getFromIni(&(wn->useFile), parse_ini_get_bool,
	           ini, "useFile", sectionName);
	getFromIni(&(wn->dumpWhiteNoise), parse_ini_get_bool,
	           ini, "dumpWhiteNoise", sectionName);

	wn->reader = NULL;
	wn->rng    = NULL;
	wn->writer = NULL;

	local_newGetInput(wn, ini, sectionName);
	local_newGetOutput(wn, ini, sectionName);

	return wn;
}

extern void
g9pWN_del(g9pWN_t *wn)
{
	assert(wn != NULL && *wn != NULL);

	if ((*wn)->rng != NULL)
		rng_del(&((*wn)->rng));
	if ((*wn)->reader != NULL)
		gridReader_del(&((*wn)->reader));
	if ((*wn)->writer != NULL)
		gridWriter_del(&((*wn)->writer));
	xfree(*wn);

	*wn = NULL;
}

extern void
g9pWN_setup(g9pWN_t       wn,
            gridRegular_t grid,
            int           idxOfDensVar)
{
	gridPatch_t patch;

	assert(wn != NULL);

	patch = gridRegular_getPatchHandle(grid, 0);

	if (wn->useFile)
		gridReader_readIntoPatchForVar(wn->reader, patch, idxOfDensVar);
	else
		local_setupFromRNG(wn, patch, idxOfDensVar);
}

extern void
g9pWN_reset(g9pWN_t wn)
{
	if (!wn->useFile) {
		rng_reset(wn->rng);
	}
}

extern void
g9pWN_dump(g9pWN_t wn, gridRegular_t grid)
{
	if (wn->dumpWhiteNoise) {
		gridWriter_activate(wn->writer);
		gridWriter_writeGridRegular(wn->writer, grid);
		gridWriter_deactivate(wn->writer);
	}
}

/*--- Implementations of local functions --------------------------------*/
static void
local_newGetInput(g9pWN_t     wn,
                  parse_ini_t ini,
                  const char  *sectionName)
{
	if (wn->useFile) {
		wn->reader = gridReader_newFromIni(ini, sectionName);
	} else {
		char *rngSectionName;
#ifndef WITH_SPRNG
		fprintf(stderr,
		        "WITH_SPRNG must be defined to use random numbers.\n");
		diediedie(EXIT_FAILURE);
#endif
		getFromIni(&rngSectionName, parse_ini_get_string,
		           ini, "rngSectionName", sectionName);
		wn->rng = rng_newFromIni(ini, rngSectionName);
		xfree(rngSectionName);
	}
}

static void
local_newGetOutput(g9pWN_t     wn,
                   parse_ini_t ini,
                   const char  *sectionName)
{
	if (wn->dumpWhiteNoise) {
		wn->writer = gridWriterFactory_newWriterFromIni(ini, sectionName);
#ifdef WITH_MPI
		gridWriter_initParallel(wn->writer, MPI_COMM_WORLD);
#endif
	} else {
		wn->writer = NULL;
	}
}

static void
local_setupFromRNG(g9pWN_t     wn,
                   gridPatch_t patch,
                   int         idxOfDensVar)
{
	int      numStreams;
	fpv_t    *data;
	uint64_t numCells = 0;

	data       = gridPatch_getVarDataHandle(patch, idxOfDensVar);
	numCells   = gridPatch_getNumCells(patch);
	numStreams = rng_getNumStreamsLocal(wn->rng);

#ifdef _OPENMP
#  pragma omp parallel for shared(data, numStreams, numCells)
#endif
	for (int i = 0; i < numStreams; i++) {
		uint64_t cps   = numCells / numStreams;
		uint64_t start = i * cps;
		uint64_t stop  = (i == numStreams - 1) ? numCells : (start
		                                                     + cps);
		for (uint64_t j = start; j < stop; j++) {
			assert(j < numCells);
			data[j] = (fpv_t)rng_getGaussUnit(wn->rng, i);
		}
	}
}
