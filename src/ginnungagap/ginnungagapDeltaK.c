// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagapDeltaK.h"
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"
#include "../libutil/utilMath.h"
#include "../libgrid/gridPoint.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridPatch.h"
#include "../libcosmo/cosmoPk.h"


/*--- Implemention of main structure ------------------------------------*/
#include "ginnungagap_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_init(ginnungagap_t     ginnungagap,
           fpvComplex_t      **data,
           gridPointUint32_t dimsGrid,
           gridPointUint32_t dimsPatch,
           gridPointUint32_t idxLo,
           gridPointUint32_t kNyquistGrid);


#ifdef WITH_MPI
static void
local_reducePk(double *pK, double *k, uint32_t *nums, uint32_t kNyquistGrid);

#endif

static void
local_dumpPk(double *P, double *k, uint32_t kNyquist, cosmoPk_t orig);


/*--- Implementations of exported functios ------------------------------*/
extern void
ginnungagapDeltaK_calcFromWhiteNoise(ginnungagap_t ginnungagap)
{
	gridPointUint32_t dimsGrid;
	gridPointUint32_t dimsPatch;
	gridPointUint32_t idxLo;
	fpvComplex_t      *data;
	double            wavenumToFreq;
	double            norm;
	gridPointUint32_t kNyquistGrid;

	assert(ginnungagap != NULL);

	local_init(ginnungagap, &data, dimsGrid, dimsPatch, idxLo, kNyquistGrid);
	norm = gridRegularFFT_getNorm(ginnungagap->gridFFT);
	wavenumToFreq = 2. * M_PI * 1. / (ginnungagap->setup->boxsizeInMpch);

#ifdef _OPENMP
#  pragma omp parallel for shared(dimsPatch, idxLo, kNyquistGrid, \
	dimsGrid, data, ginnungagap, norm)
#endif
	for (uint32_t k = 0; k < dimsPatch[2]; k++) {
		uint32_t k2 = k + idxLo[2];
		k2 = (k2 >= kNyquistGrid[2]) ? dimsGrid[2] - k2 : k2;
		for (uint32_t j = 0; j < dimsPatch[1]; j++) {
			uint32_t k1 = j + idxLo[1];
			k1 = (k1 >= kNyquistGrid[1]) ? dimsPatch[1] - k1 : k1;
			for (uint32_t i = 0; i < dimsGrid[0]; i++) {
				uint32_t k0 = i + idxLo[0];
				double   kCell;
				uint64_t idx;

				k0    = (k0 >= kNyquistGrid[0]) ? dimsGrid[0] - k0 : k0;
				idx   = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCell = sqrt(k0 * k0 + k1 * k1 + k2 * k2) * wavenumToFreq;

				if ((k0 == 0) && (k1 == 0) && (k2 == 0)) {
					data[idx] = 0.0;
				} else {
					double tmp;
					tmp = sqrt(cosmoPk_eval(ginnungagap->pk, kCell));
					data[idx] *= (fpv_t)(tmp * norm);
				}
			}
		}
	}
} /* ginnungagapDeltaK_calcFromWhiteNoise */

extern void
ginnungagapDeltaK_calcPowerSpectrum(ginnungagap_t ginnungagap)
{
	gridPointUint32_t dimsGrid;
	gridPointUint32_t dimsPatch;
	gridPointUint32_t idxLo;
	fpvComplex_t      *data;
	double            wavenumToFreq;
	gridPointUint32_t kNyquistGrid;
	double            *P;
	double            *freq;
	uint32_t          *numFreqHits;

	assert(ginnungagap != NULL);

	local_init(ginnungagap, &data, dimsGrid, dimsPatch, idxLo, kNyquistGrid);
	wavenumToFreq = 2. * M_PI * 1. / (ginnungagap->setup->boxsizeInMpch);
	P             = xmalloc(sizeof(double) * kNyquistGrid[0]);
	freq          = xmalloc(sizeof(double) * kNyquistGrid[0]);
	numFreqHits   = xmalloc(sizeof(uint32_t) * kNyquistGrid[0]);
	for (int i = 0; i < kNyquistGrid[0]; i++) {
		P[i]           = 0.0;
		freq[i]        = -1.0;
		numFreqHits[i] = 0;
	}

	for (uint32_t k = 0; k < dimsPatch[2]; k++) {
		uint32_t k2 = k + idxLo[2];
		k2 = (k2 >= kNyquistGrid[2]) ? dimsGrid[2] - k2 : k2;
		for (uint32_t j = 0; j < dimsPatch[1]; j++) {
			uint32_t k1 = j + idxLo[1];
			k1 = (k1 >= kNyquistGrid[1]) ? dimsPatch[1] - k1 : k1;
			for (uint32_t i = 0; i < dimsGrid[0]; i++) {
				uint32_t k0 = i + idxLo[0];
				int      kCell;
				uint64_t idx;

				k0    = (k0 >= kNyquistGrid[0]) ?
				        dimsGrid[0] - k0 : k0;
				idx   = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCell = (int)floor(sqrt(k0 * k0 + k1 * k1 + k2 * k2));

				if (kCell < kNyquistGrid[0]) {
					P[kCell]   += creal(data[idx]) * creal(data[idx])
					              + cimag(data[idx]) * cimag(data[idx]);
					freq[kCell] = (kCell+1) * wavenumToFreq;
					numFreqHits[kCell]++;
				}
			}
		}
	}
#ifdef WITH_MPI
	local_reducePk(P, freq, numFreqHits, kNyquistGrid[0]);
#endif
	for (uint32_t i = 0; i<kNyquistGrid[0]; i++)
		P[i] /= numFreqHits[i];

	local_dumpPk(P, freq, kNyquistGrid[0], ginnungagap->pk);

	xfree(numFreqHits);
	xfree(freq);
	xfree(P);
} /* ginnungagapDeltaK_calcPowerSpectrum */

/*--- Implementations of local functions --------------------------------*/
static void
local_init(ginnungagap_t     ginnungagap,
           fpvComplex_t      **data,
           gridPointUint32_t dimsGrid,
           gridPointUint32_t dimsPatch,
           gridPointUint32_t idxLo,
           gridPointUint32_t kNyquistGrid)
{
	gridRegular_t grid;
	gridPatch_t   patch;

	grid  = gridRegularFFT_getGridFFTed(ginnungagap->gridFFT);
	gridRegular_getDims(grid, dimsGrid);
	patch = gridRegular_getPatchHandle(grid, 0);
	gridPatch_getDims(patch, dimsPatch);
	gridPatch_getIdxLo(patch, idxLo);
	*data = gridPatch_getVarDataHandle(patch, 0);

	for (int i = 0; i < NDIM; i++) {
		kNyquistGrid[i] = ginnungagap->setup->dim1D / 2;
	}
}

#ifdef WITH_MPI
static void
local_reducePk(double *pK, double *k, uint32_t *nums, uint32_t kNyquistGrid)
{
	double *tmp;
	uint32_t *tmpInt;

	tmp    = xmalloc(sizeof(double) * kNyquistGrid);
	memcpy(tmp, pK, kNyquistGrid * sizeof(double));
	MPI_Allreduce(tmp, pK, (int)kNyquistGrid, MPI_DOUBLE, MPI_SUM,
	              MPI_COMM_WORLD);
	memcpy(tmp, k, kNyquistGrid * sizeof(double));
	MPI_Allreduce(tmp, k, (int)kNyquistGrid, MPI_DOUBLE, MPI_MAX,
	              MPI_COMM_WORLD);
	xfree(tmp);

	tmpInt = xmalloc(sizeof(uint32_t) * kNyquistGrid);
	memcpy(tmpInt, nums, kNyquistGrid * sizeof(uint32_t));
	MPI_Allreduce(tmpInt, nums, (int)kNyquistGrid, MPI_UNSIGNED,
	              MPI_SUM, MPI_COMM_WORLD);
	xfree(tmpInt);
}

#endif

static void
local_dumpPk(double *P, double *k, uint32_t kNyquist, cosmoPk_t orig)
{
	int rank = 0;
	cosmoPk_t pk;

#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	if (rank == 0) {
		pk = cosmoPk_newFromArrays(kNyquist, k, P, 0.0, 0.0);
		cosmoPk_dumpToFile(orig, "Pk.orig.dat", 1);
		cosmoPk_dumpToFile(pk, "Pk.calc.dat", 1);
		cosmoPk_del(&pk);
	}
}
