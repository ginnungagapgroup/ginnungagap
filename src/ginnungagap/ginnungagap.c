// Copyright (C) 2010, 2011, Steffen Knollmann
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
#include <stdbool.h>
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
#include "../libdata/dataVar.h"
#include "../libdata/dataVarType.h"
#include "../libgrid/gridWriter.h"
#include "../libgrid/gridStatistics.h"
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
local_doWhiteNoise(ginnungagap_t ginnungagap, bool doDumpOfWhiteNoise);

static void
local_doWhiteNoisePk(ginnungagap_t ginnungagap);

static void
local_doDeltaK(ginnungagap_t ginnungagap);

static void
local_doDeltaKPk(ginnungagap_t ginnungagap);

static void
local_doDeltaX(ginnungagap_t ginnungagap);

static void
local_doVelocities(ginnungagap_t ginnungagap, ginnungagapICMode_t mode);

static void
local_doStatistics(ginnungagap_t ginnungagap, int idxOfVar);


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
	                     ginnungagap->setup->normalisationMode);
	if (ginnungagap->rank == 0)
		printf("\n");
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	assert(ginnungagap != NULL);

	if (ginnungagap->rank == 0)
		printf("\nGenerating IC:\n\n");

	local_doWhiteNoise(ginnungagap, true);
	local_doWhiteNoisePk(ginnungagap);
	local_doDeltaK(ginnungagap);
	local_doDeltaKPk(ginnungagap);
	local_doDeltaX(ginnungagap);
	local_doStatistics(ginnungagap, 0);
	if (ginnungagap->rank == 0)
		printf("\n");

	ginnungagapWN_reset(ginnungagap->whiteNoise);
	local_doWhiteNoise(ginnungagap, false);
	local_doDeltaK(ginnungagap);
	local_doVelocities(ginnungagap, GINNUNGAGAPIC_MODE_VX);
	local_doStatistics(ginnungagap, 0);
	if (ginnungagap->rank == 0)
		printf("\n");

	ginnungagapWN_reset(ginnungagap->whiteNoise);
	local_doWhiteNoise(ginnungagap, false);
	local_doDeltaK(ginnungagap);
	local_doVelocities(ginnungagap, GINNUNGAGAPIC_MODE_VY);
	local_doStatistics(ginnungagap, 0);
	if (ginnungagap->rank == 0)
		printf("\n");

	ginnungagapWN_reset(ginnungagap->whiteNoise);
	local_doWhiteNoise(ginnungagap, false);
	local_doDeltaK(ginnungagap);
	local_doVelocities(ginnungagap, GINNUNGAGAPIC_MODE_VZ);
	local_doStatistics(ginnungagap, 0);
	if (ginnungagap->rank == 0)
		printf("\n");
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
	dataVar_t   dens;

#ifdef WITH_MPI
	localRank = gridRegularDistrib_getLocalRank(ginnungagap->gridDistrib);
#endif
	patch     = gridRegularDistrib_getPatchForRank(ginnungagap->gridDistrib,
	                                               localRank);
	gridRegular_attachPatch(ginnungagap->grid, patch);

	dens = dataVar_new("density", DATAVARTYPE_FPV, 1);
#ifdef WITH_FFT_FFTW3
#  ifdef ENABLE_DOUBLE
	dataVar_setMemFuncs(dens, &fftw_malloc, &fftw_free);
#  else
	dataVar_setMemFuncs(dens, &fftwf_malloc, &fftwf_free);
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
local_doWhiteNoise(ginnungagap_t ginnungagap, bool doDumpOfWhiteNoise)
{
	double timing;

	timing = timer_start("  Setting up white noise");
	ginnungagapWN_setup(ginnungagap->whiteNoise,
	                    ginnungagap->grid,
	                    ginnungagap->posOfDens);
	timing = timer_stop(timing);

	if (doDumpOfWhiteNoise) {
		timing = timer_start("  Writing white noise to file");
		ginnungagapWN_dump(ginnungagap->whiteNoise, ginnungagap->grid);
		timing = timer_stop(timing);
	}

	timing = timer_start("  Going to k-space");
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_FORWARD);
	timing = timer_stop(timing);
}

static void
local_doWhiteNoisePk(ginnungagap_t ginnungagap)
{
	double timing;
	cosmoPk_t pk;

	timing = timer_start("  Calculating P(k) for white noise");
	pk = ginnungagapIC_calcPkFromDelta(ginnungagap->gridFFT,
	                                   ginnungagap->setup->dim1D,
	                                   ginnungagap->setup->boxsizeInMpch);
	cosmoPk_dumpToFile(pk, "wn.pk.dat", 1);
	cosmoPk_del(&pk);
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
local_doDeltaKPk(ginnungagap_t ginnungagap)
{
	double timing;
	cosmoPk_t pk;

	timing = timer_start("  Calculating P(k) for delta(k)");
	pk = ginnungagapIC_calcPkFromDelta(ginnungagap->gridFFT,
	                                   ginnungagap->setup->dim1D,
	                                   ginnungagap->setup->boxsizeInMpch);
	cosmoPk_dumpToFile(pk, "deltak.pk.dat", 1);
	cosmoPk_del(&pk);
	timing = timer_stop(timing);
}

static void
local_doDeltaX(ginnungagap_t ginnungagap)
{
	double timing;

	timing = timer_start("  Going back to real space");
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_BACKWARD);
	timing = timer_stop(timing);

#ifdef ENABLE_WRITING
	timing = timer_start("  Writing delta(x) to file");
	gridWriter_activate(ginnungagap->finalWriter);
	gridWriter_writeGridRegular(ginnungagap->finalWriter,
	                            ginnungagap->grid);
	gridWriter_deactivate(ginnungagap->finalWriter);
	timing = timer_stop(timing);
#endif
}

static void
local_doVelocities(ginnungagap_t ginnungagap, ginnungagapICMode_t mode)
{
	double    timing;
	char      *msg = NULL, *msg2 = NULL;
#ifdef ENABLE_WRITING
	dataVar_t var;
#endif

	msg    = xstrmerge("  Generating ", ginnungagapIC_getModeStr(mode));
	msg2   = xstrmerge(msg, "(k)");
	timing = timer_start(msg2);
	ginnungagapIC_calcVelFromDelta(ginnungagap->gridFFT,
	                               ginnungagap->setup->dim1D,
	                               ginnungagap->setup->boxsizeInMpch,
	                               ginnungagap->model,
	                               cosmo_z2a(ginnungagap->setup->zInit),
	                               mode);
	timing = timer_stop(timing);
	xfree(msg2);
	xfree(msg);

	timing = timer_start("  Going back to real space");
	gridRegularFFT_execute(ginnungagap->gridFFT, GRIDREGULARFFT_BACKWARD);
	timing = timer_stop(timing);

#ifdef ENABLE_WRITING
	msg    = xstrmerge("  Writing ", ginnungagapIC_getModeStr(mode));
	msg2   = xstrmerge(msg, "(x) to file");
	timing = timer_start(msg2);
	var    = gridRegular_getVarHandle(ginnungagap->grid,
	                                  ginnungagap->posOfDens);
	dataVar_rename(var, ginnungagapIC_getModeStr(mode));
	gridWriter_activate(ginnungagap->finalWriter);
	gridWriter_writeGridRegular(ginnungagap->finalWriter,
	                            ginnungagap->grid);
	gridWriter_deactivate(ginnungagap->finalWriter);
	timing = timer_stop(timing);
	xfree(msg2);
	xfree(msg);
#endif
} /* local_doVelocities */

static void
local_doStatistics(ginnungagap_t ginnungagap, int idxOfVar)
{
	double timing;
	gridStatistics_t stat;

	timing = timer_start("  Calculating statistics");
	stat = gridStatistics_new();
	gridStatistics_calcGridRegularDistrib(stat, ginnungagap->gridDistrib,
	                                      idxOfVar);
	timing = timer_stop(timing);
	if (ginnungagap->rank == 0)
		gridStatistics_printPretty(stat, stdout, "  ");
	gridStatistics_del(&stat);
}
