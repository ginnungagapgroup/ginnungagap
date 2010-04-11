// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagap.h"
#include "ginnungagapConfig.h"
#include "fft.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/rng.h"
#include "../libutil/xmem.h"
#include "../libgrid/gridRegular.h"
#ifdef WITH_SILO
#  include "../libgrid/gridWriterSilo.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagap_struct {
	ginnungagapConfig_t config;
	rng_t               rng;
	gridRegular_t       grid;
	fft_t               fft;
	fft_t               fftConstraints;
};


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getGrid(ginnungagap_t ginnungagap);

static void
local_generateWhiteNoise(ginnungagap_t ginnungagap);


#ifdef WITH_SILO
static void
local_dumpGrid(ginnungagap_t ginnungagap);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini)
{
	ginnungagap_t ginnungagap;
	assert(ini != NULL);

	ginnungagap         = xmalloc(sizeof(struct ginnungagap_struct));
	ginnungagap->config = ginnungagapConfig_new(ini);
	ginnungagap->rng    = rng_newFromIni(ini, "rng");
	ginnungagap->fft    = fft_new(ginnungagap->config->dim1D);
	if (ginnungagap->config->useConstraints)
		ginnungagap->fftConstraints
		    = fft_new(ginnungagap->config->dim1DConstraints);
	else
		ginnungagap->fftConstraints = NULL;
	ginnungagap->grid = local_getGrid(ginnungagap);

	return ginnungagap;
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	assert(ginnungagap != NULL);

	local_generateWhiteNoise(ginnungagap);

#ifdef WITH_SILO
	local_dumpGrid(ginnungagap);
#endif
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	if ((*ginnungagap)->fftConstraints != NULL)
		fft_del(&((*ginnungagap)->fftConstraints));
	fft_del(&((*ginnungagap)->fft));
	rng_del(&((*ginnungagap)->rng));
	gridRegular_del(&((*ginnungagap)->grid));
	ginnungagapConfig_del(&((*ginnungagap)->config));
	xfree(*ginnungagap);
	*ginnungagap = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getGrid(ginnungagap_t ginnungagap)
{
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	gridVar_t         density;
	gridPatch_t       patch;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = ginnungagap->config->boxsizeInMpch;
		dims[i]   = ginnungagap->config->dim1D;
		idxLo[i]  = 0;
		idxHi[i]  = dims[i] - 1;
	}

	grid = gridRegular_new(ginnungagap->config->gridName,
	                       origin, extent, dims);

	patch   = gridPatch_new(idxLo, idxHi);
	density = gridVar_new("density", GRIDVARTYPE_FPV, 1);

	gridRegular_attachPatch(grid, patch);
	gridRegular_attachVar(grid, density);

	return grid;
}

static void
local_generateWhiteNoise(ginnungagap_t ginnungagap)
{
	gridPatch_t patch = gridRegular_getPatchHandle(
	    ginnungagap->grid,
	    0);
	fpv_t             *data    = gridPatch_getVarDataHandle(patch, 0);
	gridPointUint32_t dims;
	uint64_t          idxCells = 0;

	gridPatch_getDims(patch, dims);

	for (uint32_t iz = 0; iz < dims[0]; iz++) {
		for (uint32_t iy = 0; iy < dims[1]; iy++) {
			for (uint32_t ix = 0; ix < dims[2]; ix++) {
				data[idxCells++] = ix * ix + iy * iy + iz * iz;
			}
		}
	}

}

#ifdef WITH_SILO
static void
local_dumpGrid(ginnungagap_t ginnungagap)
{
	char             *prefix = ginnungagap->config->filePrefix;
	int              dbtype  = ginnungagap->config->dbtype;
	gridWriterSilo_t writer  = gridWriterSilo_new(prefix, dbtype);

#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, ginnungagap->config->numFiles,
	                            MPI_COMM_WORLD, 987);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, ginnungagap->grid);
	gridWriterSilo_deactivate(writer);

	gridWriterSilo_del(&writer);
}

#endif
