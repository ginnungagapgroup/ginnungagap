// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagapIC.h"
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"
#include "../libutil/utilMath.h"
#include "../libutil/diediedie.h"
#include "../libgrid/gridPoint.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridPatch.h"
#include "../libcosmo/cosmoPk.h"
#include "../libcosmo/cosmoModel.h"


/*--- Local defines -----------------------------------------------------*/

/*--- Local variables ---------------------------------------------------*/
static const char *local_modeVxStr = "velx";
static const char *local_modeVyStr = "vely";
static const char *local_modeVzStr = "velz";


/*--- Prototypes of local functions -------------------------------------*/
static void
local_getGridStuff(gridRegularFFT_t  gridFFT,
                   uint32_t          dim1D,
                   fpvComplex_t      **data,
                   gridPointUint32_t dimsGrid,
                   gridPointUint32_t dimsPatch,
                   gridPointUint32_t idxLo,
                   gridPointUint32_t kMaxGrid);


#ifdef WITH_MPI
static void
local_reducePk(double *pK, double *k, uint32_t *nums, uint32_t kNyquistGrid);

#endif

static double
local_getDisplacementToVelocityFactor(cosmoModel_t model, double aInit);


/*--- Implementations of exported functios ------------------------------*/
extern void
ginnungagapIC_calcDeltaFromWN(gridRegularFFT_t gridFFT,
                              uint32_t         dim1D,
                              double           boxsizeInMpch,
                              cosmoPk_t        pk)
{
	gridPointUint32_t dimsGrid, dimsPatch, idxLo, kMaxGrid;
	fpvComplex_t      *data;
	double            wavenumToFreq, norm;
//	double            maxFreq;

	assert(gridFFT != NULL);


	assert(pk != NULL);

	local_getGridStuff(gridFFT, dim1D, &data, dimsGrid, dimsPatch, idxLo,
	                   kMaxGrid);
	wavenumToFreq = 2. * M_PI / (boxsizeInMpch);
	norm          = sqrt(gridRegularFFT_getNorm(gridFFT));
	norm         *= pow(1. / (boxsizeInMpch), 1.5);
//	maxFreq       = 0.5 * dim1D * wavenumToFreq;

// maxFreq needs to be added to shared when used again
#ifdef _OPENMP
#  pragma omp parallel for shared(dimsPatch, idxLo, kMaxGrid, \
	dimsGrid, data, pk, norm)
#endif
	for (uint32_t k = 0; k < dimsPatch[2]; k++) {
		int64_t k2 = k + idxLo[2];
		k2 = (k2 > kMaxGrid[2]) ? k2 - dimsGrid[2] : k2;
		for (uint32_t j = 0; j < dimsPatch[1]; j++) {
			int64_t k1 = j + idxLo[1];
			k1 = (k1 > kMaxGrid[1]) ? k1 - dimsGrid[1] : k1;
			for (uint32_t i = 0; i < dimsPatch[0]; i++) {
				int64_t  k0 = i + idxLo[0];
				uint64_t idx;
				double   kCell;
				k0     = (k0 > kMaxGrid[0]) ? k0 - dimsGrid[0] : k0;
				idx    = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCell  = sqrt((double)(k0 * k0 + k1 * k1 + k2 * k2));
				kCell *= wavenumToFreq;

				if ((k0 == 0) && (k1 == 0) && (k2 == 0)) {
					data[idx] = 0.0;
//				} else if (kCell > maxFreq) {
//					data[idx] = 0.0 + 0.0I;
				} else {
					double tmp;
					tmp        = sqrt(cosmoPk_eval(pk, kCell));
//					tmp       *= cos(0.5 * M_PI * kCell / maxFreq);
					data[idx] *= (fpv_t)(tmp * norm);
				}
			}
		}
	}
} /* ginnungagapIC_calcDeltaFromWN */

extern void
ginnungagapIC_calcVelFromDelta(gridRegularFFT_t    gridFFT,
                               uint32_t            dim1D,
                               double              boxsizeInMpch,
                               cosmoModel_t        model,
                               double              aInit,
                               ginnungagapICMode_t mode)
{
	gridPointUint32_t dimsGrid, dimsPatch, idxLo, kMaxGrid;
	fpvComplex_t      *data;
	double            wavenumToFreq, norm;

	assert(gridFFT != NULL);
	assert(model != NULL);

	local_getGridStuff(gridFFT, dim1D, &data, dimsGrid, dimsPatch, idxLo,
	                   kMaxGrid);
	wavenumToFreq = 2. * M_PI / (boxsizeInMpch);
	norm          = local_getDisplacementToVelocityFactor(model, aInit);

#ifdef _OPENMP
#  pragma omp parallel for shared(dimsPatch, idxLo, kMaxGrid, \
	dimsGrid, data, norm, mode)
#endif
	for (uint32_t k = 0; k < dimsPatch[2]; k++) {
		int64_t k2 = k + idxLo[2];
		k2 = (k2 > kMaxGrid[2]) ? k2 - dimsGrid[2] : k2;
		for (uint32_t j = 0; j < dimsPatch[1]; j++) {
			int64_t k1 = j + idxLo[1];
			k1 = (k1 > kMaxGrid[1]) ? k1 - dimsGrid[1] : k1;
			for (uint32_t i = 0; i < dimsPatch[0]; i++) {
				int64_t  k0 = i + idxLo[0];
				double   kCellSqr;
				uint64_t idx;

				k0        = (k0 > kMaxGrid[0]) ? k0 - dimsGrid[0] : k0;
				idx       = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCellSqr  = (double)(k0 * k0 + k1 * k1 + k2 * k2);
				kCellSqr *= wavenumToFreq * wavenumToFreq;

				if ((k0 == 0) && (k1 == 0) && (k2 == 0)) {
					data[idx] = 0.0;
				} else if (mode == GINNUNGAGAPIC_MODE_VX) {
					data[idx] *= (fpv_t)(norm * k1 * wavenumToFreq
					                     / kCellSqr) * I;
					data[idx]  = (k1 == kMaxGrid[1]) ?
					             FPV_C(0.0) : data[idx];
				} else if (mode == GINNUNGAGAPIC_MODE_VY) {
					data[idx] *= (fpv_t)(norm * k2 * wavenumToFreq
					                     / kCellSqr) * I;
					data[idx]  = (k2 == kMaxGrid[2]) ?
					             FPV_C(0.0) : data[idx];
				} else {
					data[idx] *= (fpv_t)(norm * k0 * wavenumToFreq
					                     / kCellSqr) * I;
					data[idx]  = (k0 == kMaxGrid[0]) ?
					             FPV_C(0.0) : data[idx];
				}
			}
		}
	}
} /* ginnungagapIC_calcVelFromDelta */

extern cosmoPk_t
ginnungagapIC_calcPkFromDelta(gridRegularFFT_t gridFFT,
                              uint32_t         dim1D,
                              double           boxsizeInMpch)
{
	cosmoPk_t         pk;
	gridPointUint32_t dimsGrid, dimsPatch, idxLo, kMaxGrid;
	fpvComplex_t      *data;
	double            wavenumToFreq, *P, *freq, volume;
	uint32_t          *numFreqHits;

	assert(gridFFT != NULL);

	local_getGridStuff(gridFFT, dim1D, &data, dimsGrid, dimsPatch, idxLo,
	                   kMaxGrid);
	wavenumToFreq = 2. * M_PI * 1. / boxsizeInMpch;
	volume        = boxsizeInMpch * boxsizeInMpch * boxsizeInMpch;
	P             = xmalloc(sizeof(double) * kMaxGrid[0]);
	freq          = xmalloc(sizeof(double) * kMaxGrid[0]);
	numFreqHits   = xmalloc(sizeof(uint32_t) * kMaxGrid[0]);
	for (int i = 0; i < kMaxGrid[0]; i++) {
		P[i]           = 0.0;
		freq[i]        = -1.0;
		numFreqHits[i] = 0;
	}

	for (uint32_t k = 0; k < dimsPatch[2]; k++) {
		int64_t k2 = k + idxLo[2];
		k2 = (k2 > kMaxGrid[2]) ? k2 - dimsGrid[2] : k2;
		for (uint32_t j = 0; j < dimsPatch[1]; j++) {
			int64_t k1 = j + idxLo[1];
			k1 = (k1 > kMaxGrid[1]) ? k1 - dimsGrid[1] : k1;
			for (uint32_t i = 0; i < dimsPatch[0]; i++) {
				int64_t  k0 = i + idxLo[0];
				uint64_t idx;
				int      kCell;

				k0    = (k0 > kMaxGrid[0]) ? k0 - dimsGrid[0] : k0;
				idx   = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCell = (int)floor(sqrt((double)(k0 * k0 + k1 * k1
				                                 + k2 * k2)));

				if ((kCell <= kMaxGrid[0]) && (kCell > 0)) {
					P[kCell - 1]   += creal(data[idx]) * creal(data[idx])
					                  + cimag(data[idx]) * cimag(data[idx]);
					freq[kCell - 1] = kCell * wavenumToFreq;
					numFreqHits[kCell -1]++;
				}
			}
		}
	}
#ifdef WITH_MPI
	local_reducePk(P, freq, numFreqHits, kMaxGrid[0]);
#endif
	for (uint32_t i = 0; i < kMaxGrid[0]; i++)
		P[i] *= (volume / numFreqHits[i]);

	pk = cosmoPk_newFromArrays(kMaxGrid[0], freq, P,
	                           (P[5] - P[0]) / (freq[5] - freq[0]),
	                           (P[kMaxGrid[0] - 1] - P[kMaxGrid[0] - 6])
	                           / (freq[kMaxGrid[0] - 1]
	                              - freq[kMaxGrid[0] - 6]));

	xfree(numFreqHits);
	xfree(freq);
	xfree(P);

	return pk;
} /* ginnungagapIC_calcPowerSpectrum */

extern const char *
ginnungagapIC_getModeStr(ginnungagapICMode_t mode)
{
	const char *s;

	if (mode == GINNUNGAGAPIC_MODE_VX)
		s = local_modeVxStr;
	else if (mode == GINNUNGAGAPIC_MODE_VY)
		s = local_modeVyStr;
	else if (mode == GINNUNGAGAPIC_MODE_VZ)
		s = local_modeVzStr;
	else
		diediedie(EXIT_FAILURE);

	return s;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_getGridStuff(gridRegularFFT_t  gridFFT,
                   uint32_t          dim1D,
                   fpvComplex_t      **data,
                   gridPointUint32_t dimsGrid,
                   gridPointUint32_t dimsPatch,
                   gridPointUint32_t idxLo,
                   gridPointUint32_t kMaxGrid)
{
	gridRegular_t grid;
	gridPatch_t   patch;

	grid  = gridRegularFFT_getGridFFTed(gridFFT);
	gridRegular_getDims(grid, dimsGrid);
	patch = gridRegular_getPatchHandle(grid, 0);
	gridPatch_getDims(patch, dimsPatch);
	gridPatch_getIdxLo(patch, idxLo);
	*data = gridPatch_getVarDataHandle(patch, 0);

	for (int i = 0; i < NDIM; i++) {
		kMaxGrid[i] = dim1D / 2;
	}
	kMaxGrid[1]++; // This is the r2c FFT dimension
}

#ifdef WITH_MPI
static void
local_reducePk(double *pK, double *k, uint32_t *nums, uint32_t kMaxGrid)
{
	double   *tmp;
	uint32_t *tmpInt;

	tmp = xmalloc(sizeof(double) * kMaxGrid);
	memcpy(tmp, pK, kMaxGrid * sizeof(double));
	MPI_Allreduce(tmp, pK, (int)kMaxGrid, MPI_DOUBLE, MPI_SUM,
	              MPI_COMM_WORLD);
	memcpy(tmp, k, kMaxGrid * sizeof(double));
	MPI_Allreduce(tmp, k, (int)kMaxGrid, MPI_DOUBLE, MPI_MAX,
	              MPI_COMM_WORLD);
	xfree(tmp);

	tmpInt = xmalloc(sizeof(uint32_t) * kMaxGrid);
	memcpy(tmpInt, nums, kMaxGrid * sizeof(uint32_t));
	MPI_Allreduce(tmpInt, nums, (int)kMaxGrid, MPI_UNSIGNED,
	              MPI_SUM, MPI_COMM_WORLD);
	xfree(tmpInt);
}

#endif

static double
local_getDisplacementToVelocityFactor(cosmoModel_t model, double aInit)
{
	double error;
	double adot      = cosmoModel_calcADot(model, aInit);
	double growthVel = cosmoModel_calcDlnGrowthDlna(model, aInit, &error);

	return adot * 100. * growthVel;
}
