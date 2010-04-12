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
#if (!defined WITH_MPI && !defined _OPENMP)
#  include <time.h>
#endif
#include "../libutil/rng.h"
#include "../libutil/xmem.h"
#include "../libcosmo/cosmoModel.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridRegularDistrib.h"
#ifdef WITH_SILO
#  include "../libgrid/gridWriterSilo.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagap_struct {
	ginnungagapConfig_t  config;
	cosmoModel_t         model;
	rng_t                rng;
	gridRegular_t        grid;
	gridRegularDistrib_t gridDistrib;
	fft_t                fft;
	fft_t                fftConstraints;
};


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getGrid(ginnungagap_t ginnungagap);

static gridRegularDistrib_t
local_getGridDistrib(ginnungagap_t ginnungagap);

static void
local_initGrid(ginnungagap_t ginnungagap);

static void
local_generateWhiteNoise(ginnungagap_t ginnungagap);


#ifdef WITH_SILO
static void
local_dumpGrid(ginnungagap_t ginnungagap);

#endif

static double
local_startTimer(const char *text);

static double
local_stopTimer(double timing);


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini)
{
	ginnungagap_t ginnungagap;
	assert(ini != NULL);

	ginnungagap         = xmalloc(sizeof(struct ginnungagap_struct));
	ginnungagap->config = ginnungagapConfig_new(ini);
	ginnungagap->model  = cosmoModel_newFromIni(ini, "CosmoModel");
	ginnungagap->rng    = rng_newFromIni(ini, "rng");
	ginnungagap->fft    = fft_new(ginnungagap->config->dim1D);
	if (ginnungagap->config->useConstraints)
		ginnungagap->fftConstraints
		    = fft_new(ginnungagap->config->dim1DConstraints);
	else
		ginnungagap->fftConstraints = NULL;
	ginnungagap->grid        = local_getGrid(ginnungagap);
	ginnungagap->gridDistrib = local_getGridDistrib(ginnungagap);
	local_initGrid(ginnungagap);

	return ginnungagap;
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	double timing;
	assert(ginnungagap != NULL);

	timing = local_startTimer("Generating white noise");
	local_generateWhiteNoise(ginnungagap);
	timing = local_stopTimer(timing);

#ifdef WITH_SILO
	timing = local_startTimer("Dumping grid to silo file");
	local_dumpGrid(ginnungagap);
	timing = local_stopTimer(timing);
#endif
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	if ((*ginnungagap)->fftConstraints != NULL)
		fft_del(&((*ginnungagap)->fftConstraints));
	cosmoModel_del(&((*ginnungagap)->model));
	fft_del(&((*ginnungagap)->fft));
	rng_del(&((*ginnungagap)->rng));
	gridRegular_del(&((*ginnungagap)->grid));
	gridRegularDistrib_del(&((*ginnungagap)->gridDistrib));
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
	gridRegular_attachVar(ginnungagap->grid, dens);

	data     = gridPatch_getVarDataHandle(patch, 0);
	numCells = gridPatch_getNumCells(patch);
#ifdef _OPENMP
#  pragma omp parallel for shared(data, numCells)
#endif
	for (uint64_t i = 0; i < numCells; i++)
		data[i] = 1e10;
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
		for (uint64_t j = start; j < stop; j++)
			data[j] = (fpv_t)rng_getGaussUnit(ginnungagap->rng, i);
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

static double
local_startTimer(const char *text)
{
	double timing;
	int    rank = 0;

#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0) {
		printf("%s... ", text);
		fflush(stdout);
	}

#if (defined WITH_MPI)
	MPI_Barrier(MPI_COMM_WORLD);
	timing = -MPI_Wtime();
#elif (defined _OPENMP)
	timing = -omp_get_wtime();
#else
	timing = -clock() / CLOCKS_PER_SEC;
#endif

	return timing;
}

static double
local_stopTimer(double timing)
{
	int rank = 0;

#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

#if (defined WITH_MPI)
	timing += MPI_Wtime();
	{
		double timingMax;
		MPI_Allreduce(&timing, &timingMax, 1, MPI_DOUBLE, MPI_MAX,
		              MPI_COMM_WORLD);
		timing = timingMax;
	}
	MPI_Barrier(MPI_COMM_WORLD);
#elif (defined _OPENMP)
	timing += omp_get_wtime();
#else
	timing += clock() / CLOCKS_PER_SEC;
#endif

	if (rank == 0) {
		printf("took %.5fs\n", timing);
		fflush(stdout);
	}

	return timing;
}
