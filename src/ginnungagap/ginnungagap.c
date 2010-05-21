// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagap.h"
#include "ginnungagapConfig.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/rng.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/timer.h"
#include "../libutil/utilMath.h"
#include "../libcosmo/cosmoModel.h"
#include "../libcosmo/cosmoPk.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridRegularDistrib.h"
#include "../libgrid/gridRegularFFT.h"
#ifdef WITH_SILO
#  include "../libgrid/gridWriterSilo.h"
#endif
#ifdef ENABLE_FFT_BACKEND_FFTW3
#  include <complex.h>
#  include <fftw3.h>
#endif


/*--- Implemention of main structure ------------------------------------*/
#include <ginnungagap_adt.h>


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getGrid(ginnungagap_t ginnungagap);

static gridRegularDistrib_t
local_getGridDistrib(ginnungagap_t ginnungagap);

static void
local_initGrid(ginnungagap_t ginnungagap);

static gridRegularFFT_t
local_getFFT(ginnungagap_t ginnungagap);

static void
local_generateWhiteNoise(ginnungagap_t ginnungagap);

static void
local_realToFourier(ginnungagap_t ginnungagap);

static void
local_fourierToReal(ginnungagap_t ginnungagap);

static void
local_generateRhoK(ginnungagap_t ginnungagap);


#ifdef WITH_SILO
static void
local_dumpGrid(ginnungagap_t ginnungagap, const char *qualifier);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini)
{
	ginnungagap_t ginnungagap;

	assert(ini != NULL);

	ginnungagap              = xmalloc(sizeof(struct ginnungagap_struct));
	ginnungagap->config      = ginnungagapConfig_new(ini);
	ginnungagap->model       = cosmoModel_newFromIni(ini, "Cosmology");
	ginnungagap->pk          = cosmoPk_newFromIni(ini, "Cosmology");
	ginnungagap->rng         = rng_newFromIni(ini, "rng");
	ginnungagap->grid        = local_getGrid(ginnungagap);
	ginnungagap->gridDistrib = local_getGridDistrib(ginnungagap);
	local_initGrid(ginnungagap);
	ginnungagap->gridFFT     = local_getFFT(ginnungagap);

	return ginnungagap;
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	double timing;
	assert(ginnungagap != NULL);

	timing = timer_start("Generating white noise");
	local_generateWhiteNoise(ginnungagap);
	timing = timer_stop(timing);
#ifdef WITH_SILO
	timing = timer_start("Writing white noise to silo file");
	local_dumpGrid(ginnungagap, ".whiteNoise");
	timing = timer_stop(timing);
#endif

	timing = timer_start("Going to Fourier Space");
	local_realToFourier(ginnungagap);
	timing = timer_stop(timing);

	timing = timer_start("Generating rho(k)");
	local_generateRhoK(ginnungagap);
	timing = timer_stop(timing);

	timing = timer_start("Going back to Real Space");
	local_fourierToReal(ginnungagap);
	timing = timer_stop(timing);

#ifdef WITH_SILO
	timing = timer_start("Writing density field");
	local_dumpGrid(ginnungagap, ".density");
	timing = timer_stop(timing);
#endif
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	cosmoModel_del(&((*ginnungagap)->model));
	rng_del(&((*ginnungagap)->rng));
	gridRegularFFT_del(&((*ginnungagap)->gridFFT));
	gridRegularDistrib_del(&((*ginnungagap)->gridDistrib));
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

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = ginnungagap->config->boxsizeInMpch;
		dims[i]   = ginnungagap->config->dim1D;
	}

	grid = gridRegular_new(ginnungagap->config->gridName,
	                       origin, extent, dims);


	return grid;
}

static gridRegularDistrib_t
local_getGridDistrib(ginnungagap_t ginnungagap)
{
	gridRegularDistrib_t distrib;

	distrib = gridRegularDistrib_new(ginnungagap->grid, NULL);
#ifdef WITH_MPI
	gridRegularDistrib_initMPI(distrib, ginnungagap->config->nProcs,
	                           MPI_COMM_WORLD);
#endif

	return distrib;
}

static void
local_initGrid(ginnungagap_t ginnungagap)
{
	int         localRank = 0;
	gridPatch_t patch;
	gridVar_t   dens;
	fpv_t       *data;
	uint64_t    numCells;

#ifdef WITH_MPI
	localRank = gridRegularDistrib_getLocalRank(ginnungagap->gridDistrib);
#endif
	patch     = gridRegularDistrib_getPatchForRank(ginnungagap->gridDistrib,
	                                               localRank);
	gridRegular_attachPatch(ginnungagap->grid, patch);

	dens = gridVar_new("density", GRIDVARTYPE_FPV, 1);
#ifdef ENABLE_FFT_BACKEND_FFTW3
#  ifdef ENABLE_DOUBLE
	gridVar_setMemFuncs(dens, &fftw_malloc, &fftw_free);
#  else
	gridVar_setMemFuncs(dens, &fftwf_malloc, &fftwf_free);
#  endif
#endif
	ginnungagap->posOfDens = gridRegular_attachVar(ginnungagap->grid, dens);

	data                   = gridPatch_getVarDataHandle(patch, 0);
	numCells               = gridPatch_getNumCells(patch);
#ifdef _OPENMP
#  pragma omp parallel for shared(data, numCells)
#endif
	for (uint64_t i = 0; i < numCells; i++)
		data[i] = 1e10;
}

static gridRegularFFT_t
local_getFFT(ginnungagap_t ginnungagap)
{
	gridRegularFFT_t fft;

	fft = gridRegularFFT_new(ginnungagap->grid,
	                         ginnungagap->gridDistrib,
	                         ginnungagap->posOfDens);

	return fft;
}

static void
local_generateWhiteNoise(ginnungagap_t ginnungagap)
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

static void
local_realToFourier(ginnungagap_t ginnungagap)
{
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_FORWARD);
}

static void
local_fourierToReal(ginnungagap_t ginnungagap)
{
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_BACKWARD);
}

static void
local_generateRhoK(ginnungagap_t ginnungagap)
{
	gridRegular_t     grid;
	gridPatch_t       patch;
	gridPointUint32_t dimsGrid;
	gridPointUint32_t dimsPatch;
	gridPointUint32_t idxLo;
	fpvComplex_t      *data;
	long double       boxInv;
	uint32_t          kNyquistGrid = ginnungagap->config->dim1D / 2;

	grid  = gridRegularFFT_getGridFFTed(ginnungagap->gridFFT);
	gridRegular_getDims(grid, dimsGrid);
	patch = gridRegular_getPatchHandle(grid, 0);
	gridPatch_getDims(patch, dimsPatch);
	gridPatch_getIdxLo(patch, idxLo);
	data   = gridPatch_getVarDataHandle(patch, 0);

	boxInv = 1. / (ginnungagap->config->boxsizeInMpch);
	for (uint32_t k = 0; k < dimsPatch[2]; k++) {
		uint32_t k2 = k + idxLo[2];
		k2 = (k2 >= kNyquistGrid) ? dimsGrid[2] - k2 : k2;
		for (uint32_t j = 0; j < dimsPatch[1]; j++) {
			uint32_t k1 = j + idxLo[1];
			k1 = (k1 >= kNyquistGrid) ? dimsPatch[1] - k1 : k1;
			for (uint32_t i = 0; i < dimsGrid[0]; i++) {
				uint32_t k0 = i + idxLo[0];
				double   kCell;
				uint64_t idx;
				fpv_t tmp;
				k0     = (k0 >= kNyquistGrid) ? dimsGrid[0] - k0 : k0;
				idx    = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCell  = sqrt(k0 * k0 + k1 * k1 + k2 * k2);
				kCell *= 2. * M_PI * boxInv;

				if (k0 == 0 && k1 == 0 && k0 ==0) {
					data[idx] = 0.0;
				} else {
					tmp = sqrt(cosmoPk_eval(ginnungagap->pk, kCell));
					data[idx] *= (tmp + tmp*I);
				}
			}
		}
	}
} /* local_generateRhoK */

#ifdef WITH_SILO
static void
local_dumpGrid(ginnungagap_t ginnungagap, const char *qualifier)
{
	gridWriterSilo_t writer;
	char             *prefix = NULL;
	int              dbtype  = ginnungagap->config->dbtype;

	prefix = xstrmerge(ginnungagap->config->filePrefix, qualifier);
	writer = gridWriterSilo_new(prefix, dbtype);

#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, ginnungagap->config->numFiles,
	                            MPI_COMM_WORLD, 987);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, ginnungagap->grid);
	gridWriterSilo_deactivate(writer);

	gridWriterSilo_del(&writer);
	xfree(prefix);
}

#endif
