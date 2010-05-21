// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagapWhiteNoise.h"
#include <assert.h>
#include "../libgrid/gridPatch.h"


/*--- Implemention of main structure ------------------------------------*/
#include "ginnungagap_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
static void
ginnungagapWhiteNoise_generate(ginnungagap_t ginnungagap)
{
	gridPatch_t patch;
	fpv_t       *data;
	uint64_t    numCells = 0;
	int         numStreams;

	patch      = gridRegular_getPatchHandle(ginnungagap->grid, 0);
	data       = gridPatch_getVarDataHandle(patch, 0);
	numCells   = gridPatch_getNumCells(patch);
	numStreams = rng_getNumStreamsLocal(ginnungagap->rng);

#ifdef _OPENMP
#  pragma omp parallel for shared(data, numStreams, numCells)
#endif
	for (int i = 0; i < numStreams; i++) {
		uint64_t cps   = numCells / numStreams;
		uint64_t start = i * cps;
		uint64_t stop  = (i == numStreams - 1) ? numCells : (start + cps);
		for (uint64_t j = start; j < stop; j++) {
			assert(j < numCells);
			data[j] = (fpv_t)rng_getGaussUnit(ginnungagap->rng, i);
		}
	}
}



/*--- Implementations of local functions --------------------------------*/
