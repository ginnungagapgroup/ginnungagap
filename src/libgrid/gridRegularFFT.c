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
static gridVarType_t
local_getVarType(gridRegular_t grid, int idxFFTVar);

static void
local_getRemapping(gridRegularFFT_t fft);

static void
local_getActualFFTs(gridRegularFFT_t fft);

static void
local_getActualFFTsForward(gridRegularFFT_t fft);

static void
local_getActualFFTsBackward(gridRegularFFT_t fft);


#ifdef ENABLE_FFT_BACKEND_FFTW3
static void
local_fft_fftw3(const void *plan);

static void
local_fft_fftw3f(const void *plan);

static fftw_plan
local_getPlan_r2c_fftw3(gridRegularFFT_t fft, int idxDim);

static fftw_plan
local_getPlan_c2c_fftw3(gridRegularFFT_t fft, int idxDim);

static fftw_plan
local_getPlan_c2r_fftw3(gridRegularFFT_t fft, int idxDim);

static fftwf_plan
local_getPlan_r2c_fftw3f(gridRegularFFT_t fft, int idxDim);

static fftwf_plan
local_getPlan_c2c_fftw3f(gridRegularFFT_t fft, int idxDim);

static fftwf_plan
local_getPlan_c2r_fftw3f(gridRegularFFT_t fft, int idxDim);


#endif

/*--- Implementations of exported functios ------------------------------*/
extern gridRegularFFT_t
gridRegularFFT_new(gridRegular_t        grid,
                   gridRegularDistrib_t gridDistrib,
                   int                  idxFFTVar,
                   int                  direction)
{
	gridRegularFFT_t fft;

	assert(grid != NULL);
	assert(gridDistrib != NULL);
	assert(idxFFTVar >= 0 && idxFFTVar < gridRegular_getNumVars(grid));
	assert(direction == GRIDREGULARFFT_FORWARD
	       || direction == GRIDREGULARFFT_BACKWARD);
	assert(gridRegular_getNumPatches(grid) == 1);

	fft            = xmalloc(sizeof(struct gridRegularFFT_struct));
	fft->varType   = local_getVarType(grid, idxFFTVar);
	fft->direction = direction;
	local_getRemapping(fft);
	local_getActualFFTs(fft);

	return fft;
}

extern void
gridRegularFFT_del(gridRegularFFT_t *fft)
{
	assert(fft != NULL && *fft != NULL);

	for (int i = 0; i < NDIM; i++) {
		if ((*fft)->remapParams[i] != NULL) {
			// TODO
		}
		if ((*fft)->fftPlans[i] != NULL) {
			fftw_destroy_plan((fftw_plan)((*fft)->fftPlans[i]));
		}
	}
	xfree(*fft);

	*fft = NULL;
}

#define TOFUNC void(*)(const void *)
extern void
gridRegularFFT_execute(gridRegularFFT_t fft)
{
	assert(fft != NULL);

	for (int i = 0; i < NDIM; i++) {
		if (fft->remap[i] != NULL) {
			((TOFUNC)(fft->remap[i]))(fft->remapParams[i]);
		}
		((TOFUNC)(fft->fft1d[i]))(fft->fftPlans[i]);
	}
}
#undef TOFUNC

/*--- Implementations of local functions --------------------------------*/
static gridVarType_t
local_getVarType(gridRegular_t grid, int idxFFTVar)
{
	gridVarType_t varType;

	varType = gridVar_getType(gridRegular_getVarHandle(grid, idxFFTVar));
	assert(gridVarType_isFloating(varType));

	return varType;
}

static void
local_getRemapping(gridRegularFFT_t fft)
{
	int i;

	if (fft->direction == GRIDREGULARFFT_FORWARD) {
		fft->remap[0]       = NULL;
		fft->remapParams[0] = NULL;
		for (i = 1; i < NDIM; i++) {
			fft->remap[i]       = NULL;
			fft->remapParams[i] = NULL;
		}
	} else {
		for (i = 0; i < NDIM - 1; i++) {
			fft->remap[i]       = NULL;
			fft->remapParams[i] = NULL;
		}
		fft->remap[i]       = NULL;
		fft->remapParams[i] = NULL;
	}
}

static void
local_getActualFFTs(gridRegularFFT_t fft)
{
	if (gridVarType_isNativeDouble(fft->varType)) {
		for (int i = 0; i < NDIM; i++)
			fft->fft1d[i] = &local_fft_fftw3;
	} else {
		for (int i = 0; i < NDIM; i++)
			fft->fft1d[i] = &local_fft_fftw3f;
	}

	if (fft->direction == GRIDREGULARFFT_FORWARD)
		local_getActualFFTsForward(fft);
	else
		local_getActualFFTsBackward(fft);
}

static void
local_getActualFFTsForward(gridRegularFFT_t fft)
{
	if (gridVarType_isNativeDouble(fft->varType)) {
		fft->fftPlans[0] = (void *)local_getPlan_r2c_fftw3(fft, 0);
		for (int i = 1; i < NDIM; i++)
			fft->fftPlans[i] = (void *)local_getPlan_c2c_fftw3(fft, i);
	} else {
		fft->fftPlans[0] = (void *)local_getPlan_r2c_fftw3f(fft, 0);
		for (int i = 1; i < NDIM; i++)
			fft->fftPlans[i] = (void *)local_getPlan_c2c_fftw3f(fft, i);
	}
}

static void
local_getActualFFTsBackward(gridRegularFFT_t fft)
{
	if (gridVarType_isNativeDouble(fft->varType)) {
		for (int i = 0; i < NDIM - 1; i++)
			fft->fftPlans[i] = (void *)local_getPlan_c2c_fftw3(fft, i);
		fft->fftPlans[NDIM - 1] = (void *)local_getPlan_c2r_fftw3(fft,
		                                                          NDIM - 1);
	} else {
		for (int i = 1; i < NDIM; i++)
			fft->fftPlans[i] = (void *)local_getPlan_c2c_fftw3f(fft, i);
		fft->fftPlans[NDIM - 1] = (void *)local_getPlan_c2r_fftw3f(fft,
		                                                           NDIM - 1);
	}
}

#ifdef ENABLE_FFT_BACKEND_FFTW3
static void
local_fft_fftw3(const void *plan)
{
	fftw_execute((fftw_plan)plan);
}

static void
local_fft_fftw3f(const void *plan)
{
	fftwf_execute((fftwf_plan)plan);
}

static fftw_plan
local_getPlan_r2c_fftw3(gridRegularFFT_t fft, int idxDim)
{
	fftw_plan plan = NULL;
	assert(fft != NULL);
	assert(idxDim >= 0 && idxDim < NDIM);

	return plan;
}

static fftw_plan
local_getPlan_c2c_fftw3(gridRegularFFT_t fft, int idxDim)
{
	fftw_plan plan = NULL;
	assert(fft != NULL);
	assert(idxDim >= 0 && idxDim < NDIM);

	return plan;
}

static fftw_plan
local_getPlan_c2r_fftw3(gridRegularFFT_t fft, int idxDim)
{
	fftw_plan plan = NULL;
	assert(fft != NULL);
	assert(idxDim >= 0 && idxDim < NDIM);

	return plan;
}

static fftwf_plan
local_getPlan_r2c_fftw3f(gridRegularFFT_t fft, int idxDim)
{
	fftwf_plan plan = NULL;
	assert(fft != NULL);
	assert(idxDim >= 0 && idxDim < NDIM);

	return plan;
}

static fftwf_plan
local_getPlan_c2c_fftw3f(gridRegularFFT_t fft, int idxDim)
{
	fftwf_plan plan = NULL;
	assert(fft != NULL);
	assert(idxDim >= 0 && idxDim < NDIM);

	return plan;
}

static fftwf_plan
local_getPlan_c2r_fftw3f(gridRegularFFT_t fft, int idxDim)
{
	fftwf_plan plan = NULL;
	assert(fft != NULL);
	assert(idxDim >= 0 && idxDim < NDIM);

	return plan;
}

#endif
