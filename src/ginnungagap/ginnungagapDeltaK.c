// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagapDeltaK.h"
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include "../libutil/utilMath.h"
#include "../libgrid/gridPoint.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridPatch.h"


/*--- Implemention of main structure ------------------------------------*/
#include "ginnungagap_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern void
ginnungagapDeltaK_calcFromWhiteNoise(ginnungagap_t ginnungagap)
{
	gridRegular_t     grid;
	gridPatch_t       patch;
	gridPointUint32_t dimsGrid;
	gridPointUint32_t dimsPatch;
	gridPointUint32_t idxLo;
	fpvComplex_t      *data;
	long double       boxInv;
	uint32_t          kNyquistGrid;

	assert(ginnungagap != NULL);

	grid  = gridRegularFFT_getGridFFTed(ginnungagap->gridFFT);
	gridRegular_getDims(grid, dimsGrid);
	patch = gridRegular_getPatchHandle(grid, 0);
	gridPatch_getDims(patch, dimsPatch);
	gridPatch_getIdxLo(patch, idxLo);
	data         = gridPatch_getVarDataHandle(patch, 0);

	boxInv       = 1. / (ginnungagap->setup->boxsizeInMpch);
	kNyquistGrid = ginnungagap->setup->dim1D / 2;

#ifdef _OPENMP
#  pragma omp parallel for shared(dimsPatch, idxLo, kNyquistGrid, \
                                  dimsGrid, data, ginnungagap)
#endif
	for (uint32_t k = 0; k < dimsPatch[2]; k++) {
		uint32_t k2 = k + idxLo[2];
		k2 = (k2 >= kNyquistGrid) ? dimsGrid[2] - k2 : k2;
		for (uint32_t j = 0; j < dimsPatch[1]; j++) {
			uint32_t k1 = j + idxLo[1];
			k1 = (k1 >= kNyquistGrid) ? dimsPatch[1] - k1 : k1;
			for (uint32_t i = 0; i < dimsGrid[0]; i++) {
				uint32_t k0 = i + idxLo[0];
				double   kCell;
				uint64_t idx;
				fpv_t    tmp;
				k0     = (k0 >= kNyquistGrid) ? dimsGrid[0] - k0 : k0;
				idx    = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCell  = sqrt(k0 * k0 + k1 * k1 + k2 * k2);
				kCell *= 2. * M_PI * boxInv;

				if ((k0 == 0) && (k1 == 0) && (k0 == 0)) {
					data[idx] = 0.0;
				} else {
					tmp        = sqrt(cosmoPk_eval(ginnungagap->pk, kCell));
					data[idx] *= (tmp + tmp * I);
				}
			}
		}
	}
} /* ginnungagapDeltaK_calcFromWhiteNoise */

/*--- Implementations of local functions --------------------------------*/
