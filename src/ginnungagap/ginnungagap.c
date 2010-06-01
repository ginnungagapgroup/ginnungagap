// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagap.h"
#include "ginnungagapWhiteNoise.h"
#include "ginnungagapDeltaK.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/timer.h"
#include "../libutil/utilMath.h"
#ifdef WITH_SILO
#  include "../libgrid/gridWriterSilo.h"
#endif
#ifdef ENABLE_FFT_BACKEND_FFTW3
#  include <complex.h>
#  include <fftw3.h>
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "ginnungagap_adt.h"


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
	ginnungagap->setup       = ginnungagapSetup_new(ini);
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
	ginnungagapWhiteNoise_generate(ginnungagap);
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
	ginnungagapDeltaK_calcFromWhiteNoise(ginnungagap);
	ginnungagapDeltaK_calcPowerSpectrum(ginnungagap);
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

	cosmoPk_del(&((*ginnungagap)->pk));
	cosmoModel_del(&((*ginnungagap)->model));
	rng_del(&((*ginnungagap)->rng));
	gridRegularFFT_del(&((*ginnungagap)->gridFFT));
	gridRegularDistrib_del(&((*ginnungagap)->gridDistrib));
	gridRegular_del(&((*ginnungagap)->grid));
	ginnungagapSetup_del(&((*ginnungagap)->setup));
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
		extent[i] = ginnungagap->setup->boxsizeInMpch;
		dims[i]   = ginnungagap->setup->dim1D;
	}

	grid = gridRegular_new(ginnungagap->setup->gridName,
	                       origin, extent, dims);


	return grid;
}

static gridRegularDistrib_t
local_getGridDistrib(ginnungagap_t ginnungagap)
{
	gridRegularDistrib_t distrib;

	distrib = gridRegularDistrib_new(ginnungagap->grid, NULL);
#ifdef WITH_MPI
	gridRegularDistrib_initMPI(distrib, ginnungagap->setup->nProcs,
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
local_realToFourier(ginnungagap_t ginnungagap)
{
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_FORWARD);
}

static void
local_fourierToReal(ginnungagap_t ginnungagap)
{
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_BACKWARD);
}

#ifdef WITH_SILO
static void
local_dumpGrid(ginnungagap_t ginnungagap, const char *qualifier)
{
	gridWriterSilo_t writer;
	char             *prefix = NULL;
	int              dbtype  = ginnungagap->setup->dbtype;

	prefix = xstrmerge(ginnungagap->setup->filePrefix, qualifier);
	writer = gridWriterSilo_new(prefix, dbtype);

#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, ginnungagap->setup->numFiles,
	                            MPI_COMM_WORLD, 987);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, ginnungagap->grid);
	gridWriterSilo_deactivate(writer);

	gridWriterSilo_del(&writer);
	xfree(prefix);
}

#endif
