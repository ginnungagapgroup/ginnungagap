// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsCore.c
 * @ingroup  toolsGICSCore
 * @brief  Implements core functions for generateICs.
 */


/*--- Includes ----------------------------------------------------------*/
#include <tgmath.h>
#include "generateICsConfig.h"
#include "generateICsCore.h"
#include <assert.h>
#include <string.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/lIdx.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern void
generateICsCore_toParticles(generateICsCore_const_t d)
{
	generateICsCore_initPosID(d);
	generateICsCore_vel2pos(d);
	generateICsCore_convertVel(d);
}

extern void
generateICsCore_initPosID(generateICsCore_const_t d)
{
	gridPointUint32_t dims, idxLo;
	fpv_t             *velxP = gridPatch_getVarDataHandle(d->patch, 0);
	fpv_t             *velyP = gridPatch_getVarDataHandle(d->patch, 1);
	fpv_t             *velzP = gridPatch_getVarDataHandle(d->patch, 2);
	const double      dx     = d->data->boxsizeInMpch / d->fullDims[0];
	bool			  flg;
	gridPointUint32_t maxDims3;
	maxDims3[0]=d->maxDims; maxDims3[1]=d->maxDims; maxDims3[2]=d->maxDims;

	gridPatch_getIdxLo(d->patch, idxLo);
	gridPatch_getDims(d->patch, dims);

	printf("   Patch idxLo: (%u,%u,%u)\n", idxLo[0], idxLo[1], idxLo[2]);
	printf("   Patch dims:  (%u,%u,%u)\n", dims[0], dims[1], dims[2]);

	gridPointUint32_t p, q, pm;
	uint64_t          i = 0;
	uint64_t          iin = 0, idxM;
	uint64_t		   patchMaskDim = (dims[0]*(d->maskDim1D))/(d->partDim1D);
	for (p[2] = idxLo[2]; p[2] < idxLo[2] + dims[2]; p[2]++) {
		for (p[1] = idxLo[1]; p[1] < idxLo[1] + dims[1]; p[1]++) {
			for (p[0] = idxLo[0]; p[0] < idxLo[0] + dims[0]; p[0]++) {
				for(int j=0;j<3;j++) {
					q[j]=((p[j]-idxLo[j])*(d->maskDim1D))/(d->partDim1D);
				}
				idxM = q[0] + (q[1] + q[2]*patchMaskDim)*patchMaskDim;
				
				if(d->maskdata == NULL) flg = true;
				else flg = (d->level == d->maskdata[idxM]);
				
				if(flg) {
					//printf(" %i/%i ",i,idxM);
					d->vel[i * 3]     = velxP[iin];
					d->vel[i * 3 + 1] = velyP[iin];
					d->vel[i * 3 + 2] = velzP[iin];
					d->pos[i * 3]     = (fpv_t)( (p[0] + .5) * dx );
					d->pos[i * 3 + 1] = (fpv_t)( (p[1] + .5) * dx );
					d->pos[i * 3 + 2] = (fpv_t)( (p[2] + .5) * dx );
					for(int k=0;k<3;k++) {
						pm[k] = p[k]*((d->maxDims)/(d->partDim1D));
					}
					if (d->mode->useLongIDs) {
						( (uint64_t *)(d->id) )[i] = lIdx_fromCoord3d(pm,
						                                              maxDims3);
						//printf("%lu\n",( (uint64_t *)(d->id))[i]);
					} else {
						( (uint32_t *)(d->id) )[i] = lIdx_fromCoord3d(pm,
						                                              maxDims3);
					}
					i++;
				}
				iin++;
			}
		}
	}
	//printf(" %i/%i ",i,idxM);
} // generateICsCore_vel2pos

extern void
generateICsCore_vel2pos(generateICsCore_const_t d)
{
// uses type generic fmod, i.e. float MOD(float, float) or
// double MOD(double, double), depending on what fpv_t is
#define SCALE(d, k)                                           \
	( fmod( (fpv_t)( (d->pos[k] + d->data->vFact * d->vel[k]) \
	                 * d->data->posFactor ),                  \
	        (fpv_t)(d->data->boxsizeInMpch * d->data->posFactor) ) )
#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (uint64_t i = 0; i < d->numParticles; i++) {
		d->pos[i * 3]     += (fpv_t)(d->data->boxsizeInMpch);
		d->pos[i * 3]      = SCALE( d, (i * 3) );
		d->pos[i * 3 + 1] += (fpv_t)(d->data->boxsizeInMpch);
		d->pos[i * 3 + 1]  = SCALE( d, (i * 3 + 1) );
		d->pos[i * 3 + 2] += (fpv_t)(d->data->boxsizeInMpch);
		d->pos[i * 3 + 2]  = SCALE( d, (i * 3 + 2) );
	}
#undef SCALE
} // generateICsCore_vel2pos

extern void
generateICsCore_convertVel(generateICsCore_const_t d)
{
	double_t ainit = d->data->aInit;
	const fpv_t fac = d->data->velFactor / sqrt(ainit);
#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (uint64_t i = 0; i < d->numParticles; i++) {
		d->vel[i * 3]     *= fac;
		d->vel[i * 3 + 1] *= fac;
		d->vel[i * 3 + 2] *= fac;
	}
}

extern void
generateICsCode_dm2Gas(generateICsCore_const_t d,
                       const double            gasOffset,
                       const uint64_t          npGasTotal)
{
	const double shift     = d->data->boxsizeInMpch / d->fullDims[0]
	                         * gasOffset;
	uint64_t     npGasOrDM = d->numParticles / 2;

	printf("   Gas offset: %lf\n", shift);
	printf("   Local gas particles: %lu\n", npGasOrDM);
	printf("   Total gas particles: %lu\n", npGasTotal);

	memcpy(d->pos + 3 * npGasOrDM, d->pos, sizeof(fpv_t) * 3 * npGasOrDM);
	memcpy(d->vel + 3 * npGasOrDM, d->vel, sizeof(fpv_t) * 3 * npGasOrDM);
	if (d->mode->useLongIDs) {
		memcpy( (uint64_t *)(d->id) + npGasOrDM, d->id,
		        sizeof(uint64_t) * npGasOrDM );
	} else {
		memcpy( (uint32_t *)(d->id) + npGasOrDM, d->id,
		        sizeof(uint32_t) * npGasOrDM );
	}

#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (uint64_t i = 0; i < npGasOrDM; i++) {
		d->pos[i * 3]     += shift;
		d->pos[i * 3 + 1] += shift;
		d->pos[i * 3 + 2] += shift;
	}
#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (uint64_t i = npGasOrDM; i < d->numParticles; i++) {
		if (d->mode->useLongIDs) {
			( (uint64_t *)(d->id) )[i] += npGasTotal;
		} else {
			( (uint32_t *)(d->id) )[i] += (uint32_t)npGasTotal;
		}
	}
} // generateICsCode_dm2Gas

/*--- Implementations of local functions --------------------------------*/
