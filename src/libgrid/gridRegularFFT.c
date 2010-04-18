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


#if (!defined WITH_MPI)
static void *
local_doFFTCompletelyLocal(gridRegularFFT_t fft, int direction);

#endif

/*--- Implementations of exported functios ------------------------------*/
extern gridRegularFFT_t
gridRegularFFT_new(gridRegular_t        grid,
                   gridRegularDistrib_t gridDistrib,
                   int                  idxFFTVar)
{
	gridRegularFFT_t fft;

	assert(grid != NULL);
	assert(gridDistrib != NULL);
	assert(idxFFTVar >= 0 && idxFFTVar < gridRegular_getNumVars(grid));
	assert(gridRegular_getNumPatches(grid) == 1);

	fft          = xmalloc(sizeof(struct gridRegularFFT_struct));
	fft->varType = local_getVarType(grid, idxFFTVar);
	fft->patch   = gridRegular_getPatchHandle(grid, 0);
	fft->data    = gridPatch_getVarDataHandle(fft->patch, idxFFTVar);

	return fft;
}

extern void
gridRegularFFT_del(gridRegularFFT_t *fft)
{
	assert(fft != NULL && *fft != NULL);

	xfree(*fft);

	*fft = NULL;
}

extern void *
gridRegularFFT_execute(gridRegularFFT_t fft, int direction)
{
	assert(fft != NULL);

#if (!defined WITH_MPI)
	return local_doFFTCompletelyLocal(fft, direction);
#else
#endif
}

/*--- Implementations of local functions --------------------------------*/
static gridVarType_t
local_getVarType(gridRegular_t grid, int idxFFTVar)
{
	gridVarType_t varType;

	varType = gridVar_getType(gridRegular_getVarHandle(grid, idxFFTVar));
	assert(gridVarType_isFloating(varType));

	return varType;
}

#if (!defined WITH_MPI)
static void *
local_doFFTCompletelyLocal(gridRegularFFT_t fft, int direction)
{
#  if (defined ENABLE_FFT_BACKEND_FFTW3)
	if (gridVarType_isNativeFloat(fft->varType)) {
	} else {
		fftw_plan plan;
		if (direction == GRIDREGULARFFT_FORWARD) {
#    if (NDIM == 2)
			plan = fftw_plan_dft_r2c_2d(gridPatch_getOneDim(fft->patch, 0),
			                            gridPatch_getOneDim(fft->patch, 1),
			                            (double *)(fft->data),
			                            (fftw_complex *)(fft->data),
			                            FFTW_ESTIMATE);
#    elif (NDIM == 3)
			plan = fftw_plan_dft_r2c_3d(gridPatch_getOneDim(fft->patch, 0),
			                            gridPatch_getOneDim(fft->patch, 1),
			                            gridPatch_getOneDim(fft->patch, 2),
			                            (double *)(fft->data),
			                            (fftw_complex *)(fft->data),
			                            FFTW_ESTIMATE);
#    endif
		} else {
#    if (NDIM == 2)
			plan = fftw_plan_dft_c2r_2d(gridPatch_getOneDim(fft->patch, 0),
			                            gridPatch_getOneDim(fft->patch, 1),
			                            (fftw_complex *)(fft->data),
			                            (double *)(fft->data),
			                            FFTW_ESTIMATE);
#    elif (NDIM == 3)
			plan = fftw_plan_dft_c2r_3d(gridPatch_getOneDim(fft->patch, 0),
			                            gridPatch_getOneDim(fft->patch, 1),
			                            gridPatch_getOneDim(fft->patch, 2),
			                            (fftw_complex *)(fft->data),
			                            (double *)(fft->data),
			                            FFTW_ESTIMATE);
#    endif
		}
		fftw_execute(plan);
	}
	return fft->data;
#  endif
} /* local_doFFTCompletelyLocal */

#endif
