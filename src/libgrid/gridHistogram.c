// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridHistogram.c
 * @ingroup libgridAnalysisHisto
 * @brief  This file provides the implemenation of grid histograms.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridHistogram.h"
#include <assert.h>
#include <math.h>
#include <inttypes.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libdata/dataVar.h"
#include "../libdata/dataVarType.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridRegularDistrib.h"
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridHistogram_adt.h"


/*--- Local defines -----------------------------------------------------*/

/** @brief  Gives the largest amount of bins that are support. */
#define LOCAL_NUMBINS_MAX 4096


/*--- Prototypes of local functions -------------------------------------*/
static gridHistogram_t
local_mallocHistogram(uint32_t numBins);

static void
local_nullHistogram(gridHistogram_t histo);

static void
local_calcRegularCore(gridHistogram_t            histo,
                      const gridRegularDistrib_t distrib,
                      const gridRegular_t        grid,
                      const gridPatch_t          patch,
                      int                        idxOfVar);

static void
local_count(void *data, dataVar_t var, uint64_t len, gridHistogram_t histo);

static void
local_countValue(double value, gridHistogram_t histo);

static int
local_cmpFunc(const void *val, const void *bins);


#ifdef WITH_MPI
static void
local_mpiReduceHisto(gridHistogram_t histo, MPI_Comm comm);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern gridHistogram_t
gridHistogram_new(uint32_t numBins, double min, double max)
{
	gridHistogram_t histo;
	double          delta;

	assert(numBins > 0);
	assert(numBins <= LOCAL_NUMBINS_MAX);
	assert(min < max);

	histo               = local_mallocHistogram(numBins);

	delta               = (max - min) / numBins;
	histo->binLimits[0] = -HUGE_VAL;
	for (int i = 1; i < histo->numBins - 1; i++)
		histo->binLimits[i] = min + (i - 1) * delta;
	histo->binLimits[histo->numBins - 1] = max;
	histo->binLimits[histo->numBins]     = HUGE_VAL;

	local_nullHistogram(histo);

	return histo;
}

extern void
gridHistogram_del(gridHistogram_t *histo)
{
	assert(histo != NULL && *histo != NULL);

	if ((*histo)->binLimits != NULL)
		xfree((*histo)->binLimits);
	if ((*histo)->binCounts != NULL)
		xfree((*histo)->binCounts);
	xfree(*histo);

	*histo = NULL;
}

extern void
gridHistogram_calcGridPatch(gridHistogram_t   histo,
                            const gridPatch_t patch,
                            int               idxOfVar)
{
	assert(histo != NULL);
	assert(patch != NULL);

	local_nullHistogram(histo);
	local_calcRegularCore(histo, NULL, NULL, patch, idxOfVar);
}

extern void
gridHistogram_calcGridRegular(gridHistogram_t     histo,
                              const gridRegular_t grid,
                              int                 idxOfVar)
{
	assert(histo != NULL);
	assert(grid != NULL);

	local_nullHistogram(histo);
	local_calcRegularCore(histo, NULL, grid, NULL, idxOfVar);
}

extern void
gridHistogram_calcGridRegularDistrib(gridHistogram_t            histo,
                                     const gridRegularDistrib_t distrib,
                                     int                        idxOfVar)
{
	assert(histo != NULL);
	assert(distrib != NULL);

	local_nullHistogram(histo);
	local_calcRegularCore(histo, distrib,
	                      gridRegularDistrib_getGridHandle(distrib),
	                      NULL, idxOfVar);
}

extern uint32_t
gridHistogram_getCountInBin(const gridHistogram_t histo, uint32_t bin)
{
	assert(histo != NULL);
	assert(bin < histo->numBins);

	return histo->binCounts[bin];
}

extern double
gridHistogram_getBinLimitLeft(const gridHistogram_t histo, uint32_t bin)
{
	assert(histo != NULL);
	assert(bin < histo->numBins);

	return histo->binLimits[bin];
}

extern double
gridHistogram_getBinLimitRight(const gridHistogram_t histo, uint32_t bin)
{
	assert(histo != NULL);
	assert(bin < histo->numBins);

	return histo->binLimits[bin + 1];
}

extern void
gridHistogram_printPretty(const gridHistogram_t histo,
                          FILE                  *out,
                          const char            *prefix)
{
	assert(histo != NULL);
	assert(out != NULL);

	fprintf(out, "# Bins           :  %" PRIu32 "\n",
	        histo->numBins);
	fprintf(out, "# Used bins      :  %" PRIu32 "\n",
	        histo->numBinsReal);
	fprintf(out, "# Total count    :  %" PRIu64 "\n",
	        histo->totalCounts);
	fprintf(out, "# Counts in range:  %" PRIu64 "\n",
	        histo->totalCountsInRange);
	for (uint32_t i = 0; i < histo->numBins; i++) {
		fprintf(out, "%s %15e %15e %" PRIu32 "\n",
		        prefix != NULL ? prefix : "",
		        gridHistogram_getBinLimitLeft(histo, i),
		        gridHistogram_getBinLimitRight(histo, i),
		        gridHistogram_getCountInBin(histo, i));
	}
}

extern void
gridHistogram_printPrettyFile(const gridHistogram_t histo,
                              const char            *outFileName,
                              bool                  append,
                              const char            *prefix)
{
	FILE *f;

	if (xfile_checkIfFileExists(outFileName) && append)
		f = xfopen(outFileName, "a");
	else
		f = xfopen(outFileName, "w");

	gridHistogram_printPretty(histo, f, prefix);

	xfclose(&f);
}

extern uint32_t
gridHistogram_estimateNumBinsFromCells(uint64_t numCells)
{
	uint32_t numBins;
	double   tmp;

	tmp     = log((double)numCells)
	          * log(pow((double)numCells, 1. / ((double)NDIM)));
	numBins = (uint32_t)floor(tmp);
	if (numBins < 1)
		numBins = 1;
	if (numBins > LOCAL_NUMBINS_MAX)
		numBins = LOCAL_NUMBINS_MAX;

	return numBins;
}

/*--- Implementations of local functions --------------------------------*/
static gridHistogram_t
local_mallocHistogram(uint32_t numBins)
{
	gridHistogram_t histo;

	assert(numBins > 0);

	histo              = xmalloc(sizeof(struct gridHistogram_struct));
	histo->numBins     = numBins + 2;
	histo->numBinsReal = numBins;
	histo->binLimits   = xmalloc(sizeof(double) * (histo->numBins + 1));
	histo->binCounts   = xmalloc(sizeof(uint32_t) * (histo->numBins));

	return histo;
}

static void
local_nullHistogram(gridHistogram_t histo)
{
	for (int i = 0; i < histo->numBins; i++)
		histo->binCounts[i] = 0;
	histo->totalCounts        = UINT64_C(0);
	histo->totalCountsInRange = UINT64_C(0);
}

static void
local_calcRegularCore(gridHistogram_t            histo,
                      const gridRegularDistrib_t distrib,
                      const gridRegular_t        grid,
                      const gridPatch_t          patch,
                      int                        idxOfVar)
{
	int numPatches = 1;

	if (grid != NULL) {
		numPatches = gridRegular_getNumPatches(grid);
	}

	for (int i = 0; i < numPatches; i++) {
		gridPatch_t myPatch;
		dataVar_t   dataVar;
		void        *data;
		uint64_t    len;
		if (grid != NULL)
			myPatch = gridRegular_getPatchHandle(grid, i);
		else
			myPatch = patch;

		dataVar = gridPatch_getVarHandle(myPatch, idxOfVar);
		data    = gridPatch_getVarDataHandle(myPatch, idxOfVar);
		len     = gridPatch_getNumCells(myPatch);

		local_count(data, dataVar, len, histo);
	}

	if (distrib != NULL) {
#ifdef WITH_MPI
		MPI_Comm thisComm = gridRegularDistrib_getGlobalComm(distrib);
		local_mpiReduceHisto(histo, thisComm);
#endif
	}
}

static void
local_count(void *data, dataVar_t var, uint64_t len, gridHistogram_t histo)
{
	dataVarType_t type = dataVar_getType(var);


	switch (type) {
	case DATAVARTYPE_INT:
	{
		int *tmp = (int *)data;
		for (uint64_t i = 0; i < len; i++)
			local_countValue(tmp[i], histo);
	}
	break;
	case DATAVARTYPE_INT8:
	{
		int8_t *tmp = (int8_t *)data;
		for (uint64_t i = 0; i < len; i++)
			local_countValue(tmp[i], histo);
	}
	break;
	case DATAVARTYPE_DOUBLE:
	{
		double *tmp = (double *)data;
		for (uint64_t i = 0; i < len; i++)
			local_countValue(tmp[i], histo);
	}
	break;
	case DATAVARTYPE_FPV:
	{
		fpv_t *tmp = (fpv_t *)data;
		for (uint64_t i = 0; i < len; i++)
			local_countValue(tmp[i], histo);
	}
	break;
	}
} /* local_count */

static void
local_countValue(double value, gridHistogram_t histo)
{
	double   *leftBinEdge;
	uint32_t binNumber;

	leftBinEdge = bsearch(&value, histo->binLimits,
	                      (size_t)(histo->numBins),
	                      sizeof(double), &local_cmpFunc);
	binNumber = leftBinEdge - histo->binLimits;
	assert(binNumber < histo->numBins);
	histo->binCounts[binNumber]++;
	histo->totalCounts++;
	if ((binNumber > 0) && (binNumber < histo->numBins - 1)) {
		histo->totalCountsInRange++;
	}
}

static int
local_cmpFunc(const void *val, const void *bins)
{
	double *value        = (double *)val;
	double *leftBinLimit = (double *)bins;

	if ((value[0] >= leftBinLimit[0]) && (value[0] < leftBinLimit[1]))
		return 0;

	if (value[0] < leftBinLimit[0])
		return -1;

	return 1;
}

#ifdef WITH_MPI
static void
local_mpiReduceHisto(gridHistogram_t histo, MPI_Comm comm)
{
	uint32_t *bins;
	uint64_t countsSend[2], countsRecv[2];

	bins          = xmalloc(sizeof(uint32_t) * histo->numBins);
	countsSend[0] = histo->totalCounts;
	countsSend[1] = histo->totalCountsInRange;

	MPI_Allreduce(histo->binCounts, bins, histo->numBins, MPI_INT,
	              MPI_SUM, comm);
	MPI_Allreduce(countsSend, countsRecv, 2, MPI_LONG, MPI_SUM, comm);

	xfree(histo->binCounts);
	histo->binCounts          = bins;
	histo->totalCounts        = countsRecv[0];
	histo->totalCountsInRange = countsRecv[1];
}

#endif
