// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file ginnungagap.c
 * @ingroup  ginnungagap
 * @brief  Provides the implementation of ginnungagap.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "ginnungagap.h"
#include "g9pInit.h"
#include "g9pWN.h"
#include "g9pIC.h"
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
local_getGrid(ginnungagap_t g9p);

static gridRegularDistrib_t
local_getGridDistrib(ginnungagap_t g9p);

static void
local_initGrid(ginnungagap_t g9p);

static gridRegularFFT_t
local_getFFT(ginnungagap_t g9p);

static void
local_doWhiteNoise(ginnungagap_t g9p, bool doDumpOfWhiteNoise);

static void
local_doWhiteNoisePk(ginnungagap_t g9p);

static void
local_doDeltaK(ginnungagap_t g9p);

static void
local_doDeltaKPk(ginnungagap_t g9p);

static void
local_doDeltaX(ginnungagap_t g9p);

static void
local_doVelocities(ginnungagap_t g9p, g9pICMode_t mode);

static void
local_doStatistics(ginnungagap_t g9p, int idxOfVar);


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini)
{
	ginnungagap_t g9p;

	assert(ini != NULL);

	g9p              = xmalloc(sizeof(struct ginnungagap_struct));
	g9p->setup       = g9pSetup_new(ini);
	g9p->model       = cosmoModel_newFromIni(ini, "Cosmology");
	g9p->pk          = cosmoPk_newFromIni(ini, "Cosmology");
	g9p->whiteNoise  = g9pWN_newFromIni(ini,
	                                    "WhiteNoise");
	g9p->grid        = local_getGrid(g9p);
	g9p->gridDistrib = local_getGridDistrib(g9p);
	local_initGrid(g9p);
	g9p->gridFFT     = local_getFFT(g9p);
	g9p->finalWriter = gridWriter_newFromIni(ini, "Output");
	g9p->rank        = 0;
	g9p->size        = 1;
	g9p->numThreads  = 1;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &(g9p->rank));
	MPI_Comm_rank(MPI_COMM_WORLD, &(g9p->size));
	gridWriter_initParallel(g9p->finalWriter, MPI_COMM_WORLD);
#endif
#ifdef WITH_OPENMP
	g9p->numThreads = omp_get_num_threads();
#endif

	return g9p;
}

extern void
ginnungagap_init(ginnungagap_t g9p)
{
	assert(g9p != NULL);

	if (g9p->rank == 0)
		printf("\nInitialising:\n");
	g9pInit_init(g9p->setup->boxsizeInMpch,
	             g9p->setup->dim1D,
	             g9p->setup->zInit,
	             g9p->pk,
	             g9p->model,
	             g9p->setup->normalisationMode);
	if (g9p->rank == 0)
		printf("\n");
}

extern void
ginnungagap_run(ginnungagap_t g9p)
{
	assert(g9p != NULL);

	if (g9p->rank == 0)
		printf("\nGenerating IC:\n\n");

	local_doWhiteNoise(g9p, true);
	local_doWhiteNoisePk(g9p);
	local_doDeltaK(g9p);
	local_doDeltaKPk(g9p);
	local_doDeltaX(g9p);
	local_doStatistics(g9p, 0);
	if (g9p->rank == 0)
		printf("\n");

	g9pWN_reset(g9p->whiteNoise);
	local_doWhiteNoise(g9p, false);
	local_doDeltaK(g9p);
	local_doVelocities(g9p, G9PIC_MODE_VX);
	local_doStatistics(g9p, 0);
	if (g9p->rank == 0)
		printf("\n");

	g9pWN_reset(g9p->whiteNoise);
	local_doWhiteNoise(g9p, false);
	local_doDeltaK(g9p);
	local_doVelocities(g9p, G9PIC_MODE_VY);
	local_doStatistics(g9p, 0);
	if (g9p->rank == 0)
		printf("\n");

	g9pWN_reset(g9p->whiteNoise);
	local_doWhiteNoise(g9p, false);
	local_doDeltaK(g9p);
	local_doVelocities(g9p, G9PIC_MODE_VZ);
	local_doStatistics(g9p, 0);
	if (g9p->rank == 0)
		printf("\n");
} /* ginnungagap_run */

extern void
ginnungagap_del(ginnungagap_t *g9p)
{
	assert(g9p != NULL);
	assert(*g9p != NULL);

	cosmoPk_del(&((*g9p)->pk));
	cosmoModel_del(&((*g9p)->model));
	g9pWN_del(&((*g9p)->whiteNoise));
	gridRegularFFT_del(&((*g9p)->gridFFT));
	gridRegularDistrib_del(&((*g9p)->gridDistrib));
	gridRegular_del(&((*g9p)->grid));
	gridWriter_del(&((*g9p)->finalWriter));
	g9pSetup_del(&((*g9p)->setup));
	xfree(*g9p);
	*g9p = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getGrid(ginnungagap_t g9p)
{
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = g9p->setup->boxsizeInMpch;
		dims[i]   = g9p->setup->dim1D;
	}

	grid = gridRegular_new(g9p->setup->gridName,
	                       origin, extent, dims);


	return grid;
}

static gridRegularDistrib_t
local_getGridDistrib(ginnungagap_t g9p)
{
	gridRegularDistrib_t distrib;

	distrib = gridRegularDistrib_new(g9p->grid, NULL);
#ifdef WITH_MPI
	gridRegularDistrib_initMPI(distrib, g9p->setup->nProcs,
	                           MPI_COMM_WORLD);
#endif

	return distrib;
}

static void
local_initGrid(ginnungagap_t g9p)
{
	int         localRank = 0;
	gridPatch_t patch;
	dataVar_t   dens;

#ifdef WITH_MPI
	localRank = gridRegularDistrib_getLocalRank(g9p->gridDistrib);
#endif
	patch     = gridRegularDistrib_getPatchForRank(g9p->gridDistrib,
	                                               localRank);
	gridRegular_attachPatch(g9p->grid, patch);

	dens = dataVar_new("density", DATAVARTYPE_FPV, 1);
#ifdef WITH_FFT_FFTW3
#  ifdef ENABLE_DOUBLE
	dataVar_setMemFuncs(dens, &fftw_malloc, &fftw_free);
#  else
	dataVar_setMemFuncs(dens, &fftwf_malloc, &fftwf_free);
#  endif
#endif
	g9p->posOfDens = gridRegular_attachVar(g9p->grid, dens);
}

static gridRegularFFT_t
local_getFFT(ginnungagap_t g9p)
{
	gridRegularFFT_t fft;

	fft = gridRegularFFT_new(g9p->grid,
	                         g9p->gridDistrib,
	                         g9p->posOfDens);

	return fft;
}

static void
local_doWhiteNoise(ginnungagap_t g9p, bool doDumpOfWhiteNoise)
{
	double timing;

	timing = timer_start("  Setting up white noise");
	g9pWN_setup(g9p->whiteNoise,
	            g9p->grid,
	            g9p->posOfDens);
	timing = timer_stop(timing);

	if (doDumpOfWhiteNoise) {
		timing = timer_start("  Writing white noise to file");
		g9pWN_dump(g9p->whiteNoise, g9p->grid);
		timing = timer_stop(timing);
	}

	timing = timer_start("  Going to k-space");
	gridRegularFFT_execute(g9p->gridFFT, GRIDREGULARFFT_FORWARD);
	timing = timer_stop(timing);
}

static void
local_doWhiteNoisePk(ginnungagap_t g9p)
{
	double    timing;
	cosmoPk_t pk;

	timing = timer_start("  Calculating P(k) for white noise");
	pk     = g9pIC_calcPkFromDelta(g9p->gridFFT,
	                               g9p->setup->dim1D,
	                               g9p->setup->boxsizeInMpch);
	cosmoPk_dumpToFile(pk, "wn.pk.dat", 1);
	cosmoPk_del(&pk);
	timing = timer_stop(timing);
}

static void
local_doDeltaK(ginnungagap_t g9p)
{
	double timing;

	timing = timer_start("  Generating delta(k)");
	g9pIC_calcDeltaFromWN(g9p->gridFFT,
	                      g9p->setup->dim1D,
	                      g9p->setup->boxsizeInMpch,
	                      g9p->pk);
	timing = timer_stop(timing);
}

static void
local_doDeltaKPk(ginnungagap_t g9p)
{
	double    timing;
	cosmoPk_t pk;

	timing = timer_start("  Calculating P(k) for delta(k)");
	pk     = g9pIC_calcPkFromDelta(g9p->gridFFT,
	                               g9p->setup->dim1D,
	                               g9p->setup->boxsizeInMpch);
	cosmoPk_dumpToFile(pk, "deltak.pk.dat", 1);
	cosmoPk_del(&pk);
	timing = timer_stop(timing);
}

static void
local_doDeltaX(ginnungagap_t g9p)
{
	double timing;

	timing = timer_start("  Going back to real space");
	gridRegularFFT_execute(g9p->gridFFT, GRIDREGULARFFT_BACKWARD);
	timing = timer_stop(timing);

#ifdef ENABLE_WRITING
	timing = timer_start("  Writing delta(x) to file");
	gridWriter_activate(g9p->finalWriter);
	gridWriter_writeGridRegular(g9p->finalWriter,
	                            g9p->grid);
	gridWriter_deactivate(g9p->finalWriter);
	timing = timer_stop(timing);
#endif
}

static void
local_doVelocities(ginnungagap_t g9p, g9pICMode_t mode)
{
	double    timing;
	char      *msg = NULL, *msg2 = NULL;
#ifdef ENABLE_WRITING
	dataVar_t var;
#endif

	msg    = xstrmerge("  Generating ", g9pIC_getModeStr(mode));
	msg2   = xstrmerge(msg, "(k)");
	timing = timer_start(msg2);
	g9pIC_calcVelFromDelta(g9p->gridFFT,
	                       g9p->setup->dim1D,
	                       g9p->setup->boxsizeInMpch,
	                       g9p->model,
	                       cosmo_z2a(g9p->setup->zInit),
	                       mode);
	timing = timer_stop(timing);
	xfree(msg2);
	xfree(msg);

	timing = timer_start("  Going back to real space");
	gridRegularFFT_execute(g9p->gridFFT, GRIDREGULARFFT_BACKWARD);
	timing = timer_stop(timing);

#ifdef ENABLE_WRITING
	msg    = xstrmerge("  Writing ", g9pIC_getModeStr(mode));
	msg2   = xstrmerge(msg, "(x) to file");
	timing = timer_start(msg2);
	var    = gridRegular_getVarHandle(g9p->grid,
	                                  g9p->posOfDens);
	dataVar_rename(var, g9pIC_getModeStr(mode));
	gridWriter_activate(g9p->finalWriter);
	gridWriter_writeGridRegular(g9p->finalWriter,
	                            g9p->grid);
	gridWriter_deactivate(g9p->finalWriter);
	timing = timer_stop(timing);
	xfree(msg2);
	xfree(msg);
#endif
} /* local_doVelocities */

static void
local_doStatistics(ginnungagap_t g9p, int idxOfVar)
{
	double           timing;
	gridStatistics_t stat;

	timing = timer_start("  Calculating statistics");
	stat   = gridStatistics_new();
	gridStatistics_calcGridRegularDistrib(stat, g9p->gridDistrib,
	                                      idxOfVar);
	timing = timer_stop(timing);
	if (g9p->rank == 0)
		gridStatistics_printPretty(stat, stdout, "  ");
	gridStatistics_del(&stat);
}
