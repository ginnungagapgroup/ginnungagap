// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegularFFT.h"
#include "gridVarType.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#ifdef ENABLE_FFT_BACKEND_FFTW3
#  include <fftw3.h>
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularFFT_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
#if (defined WITH_MPI)
static void
local_initMPIStuff(gridRegularFFT_t fft);

#endif

#if (!defined WITH_MPI)
static void *
local_doFFTCompletelyLocal(gridRegularFFT_t fft, int direction);


#else
static void *
local_doFFTParallel(gridRegularFFT_t fft, int direction);

static void *
local_doFFTParallelForward(gridRegularFFT_t fft);

static void *
local_doFFTParallelR2CPencil(gridRegularFFT_t fft);

static void *
local_doFFTParallelBackward(gridRegularFFT_t fft);

#endif

/*--- Implementations of exported functios ------------------------------*/
extern gridRegularFFT_t
gridRegularFFT_new(gridRegular_t        grid,
                   gridRegularDistrib_t distrib,
                   int                  idxFFTVar)
{
	gridRegularFFT_t fft;

	assert(grid != NULL);
	assert(distrib != NULL);
	assert(idxFFTVar >= 0 && idxFFTVar < gridRegular_getNumVars(grid));
	assert(gridRegular_getNumPatches(grid) == 1);

	fft            = xmalloc(sizeof(struct gridRegularFFT_struct));
	fft->grid      = gridRegular_getRef(grid);
	fft->distrib   = gridRegularDistrib_getRef(distrib);
	fft->idxFFTVar = idxFFTVar;
	fft->var       = gridRegular_getVarHandle(grid, idxFFTVar);
	assert(gridVarType_isFloating(gridVar_getType(fft->var)));
	fft->patch     = gridRegular_getPatchHandle(grid, 0);
	gridRegularDistrib_getNProcs(fft->distrib, fft->nProcs);
#if (defined WITH_MPI)
	local_initMPIStuff(fft);
#endif

	return fft;
}

extern void
gridRegularFFT_del(gridRegularFFT_t *fft)
{
	assert(fft != NULL && *fft != NULL);

	gridRegular_del(&((*fft)->grid));
	gridRegularDistrib_del(&((*fft)->distrib));
	xfree(*fft);

	*fft = NULL;
}

extern void *
gridRegularFFT_execute(gridRegularFFT_t fft, int direction)
{
	void *result;

	assert(fft != NULL);
	assert(direction == GRIDREGULARFFT_FORWARD
	       || direction == GRIDREGULARFFT_BACKWARD);

#if (!defined WITH_MPI)
	result = local_doFFTCompletelyLocal(fft, direction);
#else
	result = local_doFFTParallel(fft, direction);
#endif
	return result;
}

/*--- Implementations of local functions --------------------------------*/
#if (defined WITH_MPI)
static void
local_initMPIStuff(gridRegularFFT_t fft)
{
	gridPointInt_t procCoords;

	gridRegularDistrib_getProcCoords(fft->distrib, procCoords);
	gridRegular_getDims(fft->grid, fft->globalDims[0]);

	fft->localNumRealElements = fft->globalDims[0][0];
	fft->globalDims[0][0]     = fft->globalDims[0][0] / 2 + 1;

	for (int i = 1; i < NDIM; i++) {
		uint32_t tmp;
		for (int j = 0; j < NDIM; j++) {
			fft->globalDims[i][j] = fft->globalDims[i - 1][j];
		}
		tmp                          = fft->globalDims[i][i % NDIM];
		fft->globalDims[i][i % NDIM] = fft->globalDims[i][0];
		fft->globalDims[i][0]        = tmp;
	}

	for (int i = 0; i < NDIM; i++) {
		for (int j = 0; j < NDIM; j++) {
			gridRegularDistrib_calcIdxsForRank1D(fft->globalDims[i][j],
			                                     fft->nProcs[j],
			                                     procCoords[j],
			                                     fft->localIdxLo[i] + j,
			                                     fft->localIdxHi[i] + j);
			fft->localDims[i][j] = fft->localIdxHi[i][j]
			                       - fft->localIdxLo[i][j] + 1;
		}
	}
}

#endif

#if (!defined WITH_MPI)
static void *
local_doFFTCompletelyLocal(gridRegularFFT_t fft, int direction)
{
#  if (defined ENABLE_FFT_BACKEND_FFTW3)
	gridPointUint32_t dims;
	int               n[NDIM];
	void              *data = gridPatch_getVarDataHandle(fft->patch,
	                                                     fft->idxFFTVar);

	gridPatch_getDims(fft->patch, dims);

	for (int i = 0; i < NDIM; i++)
		n[i] = dims[NDIM - 1 - i];

	if (gridVarType_isNativeFloat(gridVar_getType(fft->var))) {
		fftwf_plan plan;
		if (direction == GRIDREGULARFFT_FORWARD) {
			plan = fftwf_plan_dft_r2c(NDIM, n, (float *)(data),
			                          (fftwf_complex *)(data),
			                          FFTW_ESTIMATE);
		} else {
			plan = fftwf_plan_dft_c2r(NDIM, n, (fftwf_complex *)(data),
			                          (float *)(data),
			                          FFTW_ESTIMATE);
		}
		fftwf_execute(plan);
		fftwf_destroy_plan(plan);
	} else {
		fftw_plan plan;
		if (direction == GRIDREGULARFFT_FORWARD) {
			plan = fftw_plan_dft_r2c(NDIM, n, (double *)(data),
			                         (fftw_complex *)(data),
			                         FFTW_ESTIMATE);
		} else {
			plan = fftw_plan_dft_c2r(NDIM, n, (fftw_complex *)(data),
			                         (double *)(data),
			                         FFTW_ESTIMATE);
		}
		fftw_execute(plan);
		fftw_destroy_plan(plan);
	}
	return data;
#  endif
} /* local_doFFTCompletelyLocal */

#else
static void *
local_doFFTParallel(gridRegularFFT_t fft, int direction)
{
	void *result;
	if (direction == GRIDREGULARFFT_FORWARD)
		result = local_doFFTParallelForward(fft);
	else
		result = local_doFFTParallelBackward(fft);

	return result;
}

static void *
local_doFFTParallelForward(gridRegularFFT_t fft)
{
	void *result;

	result = local_doFFTParallelR2CPencil(fft);
	gridPatch_replaceVarData(fft->patch, fft->idxFFTVar, result);
	gridRegularDistrib_transposeVar(fft->distrib, fft->idxFFTVar, 0, 1);
//	result = local_doFFTParallelC2CPencil(fft);

	return result;
}

static void *
local_doFFTParallelR2CPencil(gridRegularFFT_t fft)
{
	int  howmany = 1;
	void *result;
	void *data   = gridPatch_getVarDataHandle(fft->patch, fft->idxFFTVar);

	for (int i = 1; i < NDIM; i++)
		howmany *= fft->localDims[0][i];

	if (gridVarType_isNativeFloat(gridVar_getType(fft->var))) {
		fftwf_plan plan;
		result = fftwf_malloc(sizeof(fftwf_complex) * howmany
		                      * fft->localDims[0][0]);
		plan   = fftwf_plan_many_dft_r2c(1, &(fft->localNumRealElements),
		                                 howmany, (float *)data,
		                                 NULL, 1, fft->localNumRealElements,
		                                 (fftwf_complex *)result,
		                                 NULL, 1, fft->localDims[0][0],
		                                 FFTW_ESTIMATE);
		fftwf_execute(plan);
		fftwf_destroy_plan(plan);
	} else {
		fftw_plan plan;
		result = fftw_malloc(sizeof(fftw_complex) * howmany
		                     * fft->localDims[0][0]);
		plan   = fftw_plan_many_dft_r2c(1, &(fft->localNumRealElements),
		                                howmany, (double *)data,
		                                NULL, 1, fft->localNumRealElements,
		                                (fftw_complex *)result,
		                                NULL, 1, fft->localDims[0][0],
		                                FFTW_ESTIMATE);
		fftw_execute(plan);
		fftw_destroy_plan(plan);
	}

	return result;
} /* local_doFFTParallelR2CPencil */

static void *
local_doFFTParallelBackward(gridRegularFFT_t fft)
{
	void *result = NULL;

	return result;
}

#endif
