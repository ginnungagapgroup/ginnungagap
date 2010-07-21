// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagap.h"
#include "ginnungagapInit.h"
#include "ginnungagapWN.h"
#include "ginnungagapIC.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_OPENMP
#  include <omp.h>
#endif
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/xfile.h"
#include "../libutil/timer.h"
#include "../libutil/utilMath.h"
#include "../libcosmo/cosmo.h"
#include "../libcosmo/cosmoFunc.h"
#include "../libgrid/gridWriter.h"
#ifdef WITH_FFT_FFTW3
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
local_doWhiteNoise(ginnungagap_t ginnungagap);

static void
local_doDeltaK(ginnungagap_t ginnungagap);

static void
local_doDeltaX(ginnungagap_t ginnungagap);

static void
local_doVelocities(ginnungagap_t ginnungagap, ginnungagapICMode_t mode);


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
	ginnungagap->whiteNoise  = ginnungagapWN_newFromIni(ini,
	                                                    "WhiteNoise");
	ginnungagap->grid        = local_getGrid(ginnungagap);
	ginnungagap->gridDistrib = local_getGridDistrib(ginnungagap);
	local_initGrid(ginnungagap);
	ginnungagap->gridFFT     = local_getFFT(ginnungagap);
	ginnungagap->finalWriter = gridWriter_newFromIni(ini, "Output");
	ginnungagap->rank        = 0;
	ginnungagap->size        = 1;
	ginnungagap->numThreads  = 1;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &(ginnungagap->rank));
	MPI_Comm_rank(MPI_COMM_WORLD, &(ginnungagap->size));
	gridWriter_initParallel(ginnungagap->finalWriter, MPI_COMM_WORLD);
#endif
#ifdef WITH_OPENMP
	ginnungagap->numThreads = omp_get_num_threads();
#endif

	return ginnungagap;
}

extern void
ginnungagap_init(ginnungagap_t ginnungagap)
{
	if (ginnungagap->rank == 0)
		printf("\nInitialising:\n");
	ginnungagapInit_init(ginnungagap->setup->boxsizeInMpch,
	                     ginnungagap->setup->dim1D,
	                     ginnungagap->setup->zInit,
	                     ginnungagap->pk,
	                     ginnungagap->model,
	                     ginnungagap->setup->forceSigma8InBox);
	if (ginnungagap->rank == 0)
		printf("\n");
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	assert(ginnungagap != NULL);

	if (ginnungagap->rank == 0)
		printf("\nGenerating IC:\n\n");

	local_doWhiteNoise(ginnungagap);
	local_doDeltaK(ginnungagap);
	local_doDeltaX(ginnungagap);
	if (ginnungagap->rank == 0)
		printf("\n");

	ginnungagapWN_reset(ginnungagap->whiteNoise);
	local_doWhiteNoise(ginnungagap);
	local_doDeltaK(ginnungagap);
	local_doVelocities(ginnungagap, GINNUNGAGAPIC_MODE_VX);
	if (ginnungagap->rank == 0)
		printf("\n");

	ginnungagapWN_reset(ginnungagap->whiteNoise);
	local_doWhiteNoise(ginnungagap);
	local_doDeltaK(ginnungagap);
	local_doVelocities(ginnungagap, GINNUNGAGAPIC_MODE_VY);
	if (ginnungagap->rank == 0)
		printf("\n");

	ginnungagapWN_reset(ginnungagap->whiteNoise);
	local_doWhiteNoise(ginnungagap);
	local_doDeltaK(ginnungagap);
	local_doVelocities(ginnungagap, GINNUNGAGAPIC_MODE_VZ);

	if (ginnungagap->rank == 0)
		printf("\n");
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	cosmoPk_del(&((*ginnungagap)->pk));
	cosmoModel_del(&((*ginnungagap)->model));
	ginnungagapWN_del(&((*ginnungagap)->whiteNoise));
	gridRegularFFT_del(&((*ginnungagap)->gridFFT));
	gridRegularDistrib_del(&((*ginnungagap)->gridDistrib));
	gridRegular_del(&((*ginnungagap)->grid));
	gridWriter_del(&((*ginnungagap)->finalWriter));
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

#ifdef WITH_MPI
	localRank = gridRegularDistrib_getLocalRank(ginnungagap->gridDistrib);
#endif
	patch     = gridRegularDistrib_getPatchForRank(ginnungagap->gridDistrib,
	                                               localRank);
	gridRegular_attachPatch(ginnungagap->grid, patch);

	dens = gridVar_new("density", GRIDVARTYPE_FPV, 1);
#ifdef WITH_FFT_FFTW3
#  ifdef ENABLE_DOUBLE
	gridVar_setMemFuncs(dens, &fftw_malloc, &fftw_free);
#  else
	gridVar_setMemFuncs(dens, &fftwf_malloc, &fftwf_free);
#  endif
#endif
	ginnungagap->posOfDens = gridRegular_attachVar(ginnungagap->grid, dens);
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
local_doWhiteNoise(ginnungagap_t ginnungagap)
{
	double timing;

	timing = timer_start("  Setting up white noise");
	ginnungagapWN_setup(ginnungagap->whiteNoise,
	                    ginnungagap->grid,
	                    ginnungagap->posOfDens);
	timing = timer_stop(timing);
	ginnungagapWN_dump(ginnungagap->whiteNoise, ginnungagap->grid);

	timing = timer_start("  Going to Fourier Space");
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_FORWARD);
	timing = timer_stop(timing);
}

static void
local_doDeltaK(ginnungagap_t ginnungagap)
{
	double timing;

	timing = timer_start("  Generating delta(k)");
	ginnungagapIC_calcDeltaFromWN(ginnungagap->gridFFT,
	                              ginnungagap->setup->dim1D,
	                              ginnungagap->setup->boxsizeInMpch,
	                              ginnungagap->pk);
	timing = timer_stop(timing);
}

static void
local_doDeltaX(ginnungagap_t ginnungagap)
{
	double timing;

	timing = timer_start("  FFTing to delta(x)");
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_BACKWARD);
	timing = timer_stop(timing);

	timing = timer_start("  Writing delta to file");
	gridWriter_activate(ginnungagap->finalWriter);
	gridWriter_writeGridRegular(ginnungagap->finalWriter,
	                            ginnungagap->grid);
	gridWriter_deactivate(ginnungagap->finalWriter);
	timing = timer_stop(timing);
}

static void
local_doVelocities(ginnungagap_t ginnungagap, ginnungagapICMode_t mode)
{
	double timing;
	char   *msg = NULL;

	msg    = xstrmerge("  Generating ", ginnungagapIC_getModeStr(mode));
	timing = timer_start(msg);
	ginnungagapIC_calcVelFromDelta(ginnungagap->gridFFT,
	                               ginnungagap->setup->dim1D,
	                               ginnungagap->setup->boxsizeInMpch,
	                               ginnungagap->model,
	                               cosmo_z2a(ginnungagap->setup->zInit),
	                               GINNUNGAGAPIC_MODE_VX);
	timing = timer_stop(timing);
	xfree(msg);

	timing = timer_start("  FFTing to vel(x)");
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_BACKWARD);
	timing = timer_stop(timing);

	timing = timer_start("  Writing velocity to file");
	gridWriter_activate(ginnungagap->finalWriter);
	gridWriter_writeGridRegular(ginnungagap->finalWriter,
	                            ginnungagap->grid);
	gridWriter_deactivate(ginnungagap->finalWriter);
	timing = timer_stop(timing);
}
