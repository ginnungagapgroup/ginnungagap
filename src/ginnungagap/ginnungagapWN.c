// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagapWN.h"
#include <assert.h>
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridReader.h"
#include "../libgrid/gridPatch.h"


/*--- Implemention of main structure ------------------------------------*/
#include "ginnungagapWN_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_setupFromRNG(ginnungagapWN_t wn,
                   gridPatch_t     patch,
                   int             idxOfDensVar);


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagapWN_t
ginnungagapWN_newFromIni(parse_ini_t ini, const char *sectionName)
{
	ginnungagapWN_t wn;

	assert(ini != NULL);
	assert(sectionName != NULL);

	wn = xmalloc(sizeof(struct ginnungagapWN_struct));
	getFromIni(&(wn->useFile), parse_ini_get_bool,
	           ini, "useFile", sectionName);

	wn->reader = NULL;
	wn->rng    = NULL;

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

	return wn;
}

extern void
ginnungagapWN_del(ginnungagapWN_t *wn)
{
	assert(wn != NULL && *wn != NULL);

	if ((*wn)->rng != NULL)
		rng_del(&((*wn)->rng));
	xfree(*wn);

	*wn = NULL;
}

extern void
ginnungagapWN_setup(ginnungagapWN_t wn,
                    gridRegular_t   grid,
                    int             idxOfDensVar)
{
	gridPatch_t patch;

	assert(wn != NULL);

	patch = gridRegular_getPatchHandle(grid, 0);

	if (wn->useFile)
		gridReader_readIntoPatchForVar(wn->reader, patch, idxOfDensVar);
	else
		local_setupFromRNG(wn, patch, idxOfDensVar);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_setupFromRNG(ginnungagapWN_t wn,
                   gridPatch_t     patch,
                   int             idxOfDensVar)
{
	int      numStreams;
	fpv_t    *data;
	uint64_t numCells = 0;

	data       = gridPatch_getVarDataHandle(patch, 0);
	numCells   = gridPatch_getNumCells(patch);
	numStreams = rng_getNumStreamsLocal(wn->rng);

#  ifdef _OPENMP
#    pragma omp parallel for shared(data, numStreams, numCells)
#  endif
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
