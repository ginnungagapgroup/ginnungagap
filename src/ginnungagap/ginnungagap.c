// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagap.h"
#include "ginnungagapWN.h"
#include "ginnungagapDeltaK.h"
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
local_realToFourier(ginnungagap_t ginnungagap);

static void
local_fourierToReal(ginnungagap_t ginnungagap);

static void
local_initPk(ginnungagap_t ginnungagap);


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
	if (ginnungagap->rank == 0) {
		printf("Initializing the run\n--------------------\n");
		printf("Box size:         %f [Mpc/h]\n",
		       ginnungagap->setup->boxsizeInMpch);
		printf("Grid resolution:  %" PRIu32 "^%i = %" PRIu64 " cells\n",
		       ginnungagap->setup->dim1D, NDIM,
		       (uint64_t)(POW_NDIM(ginnungagap->setup->dim1D)));
	}

	local_initPk(ginnungagap);

	if (ginnungagap->rank == 0) {
		printf("\n\n");
	}
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	double timing;
	assert(ginnungagap != NULL);

	if (ginnungagap->rank == 0) {
		printf("Starting the work\n-----------------\n");
	}

	timing = timer_start("Setting up white noise");
	ginnungagapWN_setup(ginnungagap->whiteNoise,
	                    ginnungagap->grid,
	                    ginnungagap->posOfDens);
	timing = timer_stop(timing);
	ginnungagapWN_dump(ginnungagap->whiteNoise, ginnungagap->grid);

	timing = timer_start("Going to Fourier Space");
	local_realToFourier(ginnungagap);
	timing = timer_stop(timing);

	timing = timer_start("Generating rho(k)");
	ginnungagapDeltaK_calcFromWhiteNoise(ginnungagap);
	ginnungagapDeltaK_calcPowerSpectrum(ginnungagap);
	timing = timer_stop(timing);

	timing = timer_start("Going back to Real Space");
	local_fourierToReal(ginnungagap);
#if 0
	{
		gridPatch_t patch = gridRegular_getPatchHandle(ginnungagap->grid, 0);
		fpv_t *data = gridPatch_getVarDataHandle(patch, 0);
		FILE *f;

		f = fopen("delta.dat", "wb");
		fwrite(data, sizeof(fpv_t), 256*256*256, f);
		fclose(f);
	}
#endif
	timing = timer_stop(timing);

	timing = timer_start("Writing final output");
	gridWriter_activate(ginnungagap->finalWriter);
	gridWriter_writeGridRegular(ginnungagap->finalWriter,
	                            ginnungagap->grid);
	gridWriter_deactivate(ginnungagap->finalWriter);
	timing = timer_stop(timing);
} /* ginnungagap_run */

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
	fpv_t       *data;
	uint64_t    numCells;

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

static void
local_initPk(ginnungagap_t ginnungagap)
{
	double kmin, kmax, kminBox, kmaxBox, sigma8, error;

	kmin  = kmax = M_PI / ginnungagap->setup->boxsizeInMpch;
	kmin *= 2.;
	kmax *= ginnungagap->setup->dim1D;
	kminBox = kmin;
	kmaxBox = kmax;
	cosmoPk_findKWindowForSigma8(ginnungagap->pk, &kmin, &kmax);

	if (ginnungagap->rank == 0) {
		printf("Getting P(k) at z = 0.0:\n");
		printf("  k_min_box = %e [h/Mpc]\n", kminBox);
		printf("  k_max_box = %e [h/Mpc]\n", kmaxBox);
		printf("  k_min = %e [h/Mpc]\n", kmin);
		printf("  k_max = %e [h/Mpc]\n", kmax);
	}

	if (ginnungagap->setup->forceSigma8InBox) {
		double correction, errorRenorm;
		sigma8     = cosmoPk_calcSigma8(ginnungagap->pk, kmin, kmax, &error);
		if (ginnungagap->rank == 0) {
			printf("  sigma8 of input P(k) = %e (sigma8^2 = %e)\n",
			       sigma8, sigma8*sigma8);
		}
		sigma8     = cosmoModel_getSigma8(ginnungagap->model);
		correction = cosmoPk_forceSigma8(ginnungagap->pk, sigma8,
		                                 kmin, kmax, &errorRenorm);
		if (ginnungagap->rank == 0) {
			printf("  Renormalizing the power spectrum:\n");
			printf("    Scaled the initial power spectrum by %e\n",
			       correction);
			printf("    sigma8 achieved with a relative error of %e\n",
			       errorRenorm);
		}
	} else {
		if (ginnungagap->rank == 0) {
			printf("  Trusting the input power spectrum to be correct\n");
		}
	}
	sigma8 = cosmoPk_calcSigma8(ginnungagap->pk, kmin, kmax, &error);
	if (ginnungagap->rank == 0) {
		printf("  In [k_min,k_max]: sigma8 = %e +- %e\n", sigma8, error);
		cosmoPk_dumpToFile(ginnungagap->pk, "Pk.final.dat", 5);
	}

	double growth0, growth1, scale, sigma;
	double one = 1.0;
	if (ginnungagap->rank == 0) {
		printf("Getting P(k) at z = %.1f:\n", ginnungagap->setup->zInit);
	}
	growth0 = cosmoModel_calcGrowth(ginnungagap->model, 1.0, &error);
	growth1 = cosmoModel_calcGrowth(ginnungagap->model,
	                                cosmo_z2a(ginnungagap->setup->zInit),
	                                &error);
	scale  = growth1 / growth0;
	scale *= scale;
	if (ginnungagap->rank == 0) {
		printf("  D0 = D(z=  0.0) = %e\n", growth0);
		printf("  D1 = D(z=%5.1f) = %e\n",
		       ginnungagap->setup->zInit, growth1);
		printf("  Scaling by (D1/D0)^2 = %e\n",
		       scale);
	}
	cosmoPk_scale(ginnungagap->pk, scale);
	sigma = sqrt(cosmoPk_calcMomentFiltered(ginnungagap->pk,
	                                        0, &cosmoFunc_const,
	                                        &one, M_SQRT1_2 * kminBox,
	                                        kmaxBox, &error));
	if (ginnungagap->rank == 0) {
		printf("  sigma in [k_min_box/sqrt(2),k_max_box] = %.4f\n",
		       sigma);
		cosmoPk_dumpToFile(ginnungagap->pk, "Pk.initial.dat", 5);
	}
} /* local_initPk */
