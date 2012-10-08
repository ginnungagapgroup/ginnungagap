// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "grafic2gadgetConfig.h"
#include "grafic2gadget.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include "../../src/libcosmo/cosmo.h"
#include "../../src/libcosmo/cosmoModel.h"
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/grafic.h"
#include "../../src/libutil/gadget.h"
#include "../../src/libutil/xfile.h"
#include "../../src/libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "grafic2gadget_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
static void
local_getFactors(grafic_t     grafic,
                 uint32_t     np[3],
                 double       *dx,
                 double       *boxsize,
                 double       *vFact,
                 double       *aInit,
                 cosmoModel_t *model,
                 double       omegaBaryon0);

static gadgetHeader_t
local_getBaseHeader(gadget_t     gadget,
                    double       boxsize,
                    double       aInit,
                    cosmoModel_t model,
                    uint32_t     np[3],
                    bool         doGas,
                    bool         useLongIDs,
                    double       posFactor);

static gadgetTOC_t
local_getTOC(void);

static void
local_getSlabNumbers(int i,
                     int numFiles,
                     int range,
                     int *numSlabStart,
                     int *numSlabEnd,
                     int *numSlabs);

static void
local_initposid(float    *pos,
                uint32_t *id,
                uint32_t np[3],
                int      zStart,
                int      zEnd,
                double   dx,
                bool     doGas);

static void
local_initposidLong(float    *pos,
                    uint64_t *id,
                    uint32_t np[3],
                    int      zStart,
                    int      zEnd,
                    double   dx,
                    bool     doGas);

static void
local_vel2pos(float    *vel,
              float    *pos,
              uint64_t num,
              double   boxsize,
              double   vFact,
              double   posFactor);

static void
local_convertVel(float *vel, uint64_t num, double aInit, double velFactor);

static void
local_checkForIDOverflow(uint32_t np[3], bool useLongIDs, bool doGas);


/*--- Implementations of exported functios ------------------------------*/
extern grafic2gadget_t
grafic2gadget_new(const char *graficFileNameVx,
                  const char *graficFileNameVy,
                  const char *graficFileNameVz,
                  const char *outputFileStem,
                  int        numOutFiles,
                  bool       force,
                  bool       useLong,
                  double     omegaBaryon0,
                  double     posFactor,
                  double     velFactor)
{
	grafic2gadget_t g2g;

	assert(graficFileNameVx != NULL);
	assert(graficFileNameVy != NULL);
	assert(graficFileNameVz != NULL);
	assert(outputFileStem != NULL);
	assert(numOutFiles > 0);

	g2g                   = xmalloc(sizeof(struct grafic2gadget_struct));
	g2g->graficFileNameVx = xstrdup(graficFileNameVx);
	g2g->graficFileNameVy = xstrdup(graficFileNameVy);
	g2g->graficFileNameVz = xstrdup(graficFileNameVz);
	g2g->numGadgetFiles   = numOutFiles;
	g2g->gadgetFileStem   = xstrdup(outputFileStem);
	g2g->force            = force;
	g2g->useLongIDs       = useLong;
	g2g->doGas            = isgreater(omegaBaryon0, 0.0) ? true : false;
	g2g->omegaBaryon0     = g2g->doGas ? omegaBaryon0 : 0.0;
	g2g->posFactor        = posFactor;
	g2g->velFactor        = velFactor;

	return g2g;
}

extern void
grafic2gadget_del(grafic2gadget_t *g2g)
{
	assert(g2g != NULL);
	assert(*g2g != NULL);

	xfree((*g2g)->graficFileNameVx);
	xfree((*g2g)->graficFileNameVy);
	xfree((*g2g)->graficFileNameVz);
	xfree((*g2g)->gadgetFileStem);
	xfree(*g2g);
	*g2g = NULL;
}

extern void
grafic2gadget_run(grafic2gadget_t g2g)
{
	grafic_t       gvx, gvy, gvz;
	uint32_t       np[3];
	uint64_t       numPlane;
	double         dx, boxsize, vFact, aInit;
	cosmoModel_t   model;
	gadget_t       gadget;
	gadgetHeader_t baseHeader;
	gadgetTOC_t    toc;

	assert(g2g != NULL);

	gvx    = grafic_newFromFile(g2g->graficFileNameVx);
	gvy    = grafic_newFromFile(g2g->graficFileNameVy);
	gvz    = grafic_newFromFile(g2g->graficFileNameVz);

	gadget = gadget_newSimple(g2g->gadgetFileStem, g2g->numGadgetFiles);
	gadget_setFileVersion(gadget, GADGETVERSION_ONE);

	local_getFactors(gvx, np, &dx, &boxsize, &vFact, &aInit, &model,
	                 g2g->omegaBaryon0);
	local_checkForIDOverflow(np, g2g->useLongIDs, g2g->doGas);
	baseHeader = local_getBaseHeader(gadget, boxsize, aInit, model, np,
	                                 g2g->doGas, g2g->useLongIDs,
	                                 g2g->posFactor);
	toc        = local_getTOC();

	numPlane   = np[0] * np[1];
	for (int i = 0; i < g2g->numGadgetFiles; i++) {
		int            numSlabStart, numSlabEnd, numSlabs;
		int64_t        numLocal;
		uint64_t       numLocalParticles;
		float          *vel, *pos;
		void           *id;
		uint32_t       npLocal[6] = {0, 0, 0, 0, 0, 0};
		double         massArr[6] = {0., 0., 0., 0., 0., 0.};
		gadgetHeader_t myHeader;

		local_getSlabNumbers(i, g2g->numGadgetFiles, np[2],
		                     &numSlabStart, &numSlabEnd, &numSlabs);
		myHeader   = gadgetHeader_clone(baseHeader);
		numLocal   = numSlabs * numPlane;
		npLocal[1] = numLocal;
		if (g2g->doGas)
			npLocal[0] = numLocal;
		numLocalParticles = npLocal[0] + npLocal[1];
		gadgetHeader_setNp(myHeader, npLocal);
		gadgetTOC_calcSizes(toc, npLocal, massArr, false, g2g->useLongIDs);
		gadgetTOC_calcOffset(toc);
		gadget_setHeaderOfFile(gadget, i, myHeader);
		gadget_setTOCOfFile(gadget, i, gadgetTOC_clone(toc));

		vel = xmalloc(sizeof(float) * numLocalParticles * 3);
		pos = xmalloc(sizeof(float) * numLocalParticles * 3);
		if (g2g->useLongIDs)
			id = xmalloc(sizeof(uint64_t) * numLocalParticles);
		else
			id = xmalloc(sizeof(uint32_t) * numLocalParticles);

		for (int j = numSlabStart; j <= numSlabEnd; j++) {
			grafic_readSlab(gvx,
			                vel + 3 * (j - numSlabStart) * numPlane,
			                GRAFIC_FORMAT_FLOAT, 3, j);
			grafic_readSlab(gvy,
			                vel + 3 * (j - numSlabStart) * numPlane + 1,
			                GRAFIC_FORMAT_FLOAT, 3, j);
			grafic_readSlab(gvz,
			                vel + 3 * (j - numSlabStart) * numPlane + 2,
			                GRAFIC_FORMAT_FLOAT, 3, j);
		}
		if (g2g->doGas)
			memcpy(vel + 3 * numLocal, vel, 3 * numLocal * sizeof(float));
		if (g2g->useLongIDs)
			local_initposidLong(pos, (uint64_t *)id, np,
			                    numSlabStart, numSlabEnd, dx,
			                    g2g->doGas);
		else
			local_initposid(pos, (uint32_t *)id, np,
			                numSlabStart, numSlabEnd, dx,
			                g2g->doGas);
		local_vel2pos(vel, pos, numLocalParticles, boxsize, vFact,
		              g2g->posFactor);
		local_convertVel(vel, numLocalParticles, aInit, g2g->velFactor);

		gadget_open(gadget, GADGET_MODE_WRITE_CREATE, i);
		gadget_writeHeaderToCurrentFile(gadget);
		{
			stai_t stai;
			stai = stai_new(pos, 3 * sizeof(float), 3 * sizeof(float));
			gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_POS_,
			                               0, numLocalParticles, stai);
			stai_del(&stai);
			stai = stai_new(vel, 3 * sizeof(float), 3 * sizeof(float));
			gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_VEL_,
			                               0, numLocalParticles, stai);
			stai_del(&stai);
			if (g2g->useLongIDs)
				stai = stai_new(id, sizeof(uint64_t), sizeof(uint64_t));
			else
				stai = stai_new(id, sizeof(uint32_t), sizeof(uint32_t));
			gadget_writeBlockToCurrentFile(gadget, GADGETBLOCK_ID__,
			                               0, numLocalParticles, stai);
			stai_del(&stai);
		}
		gadget_close(gadget);

		xfree(id);
		xfree(pos);
		xfree(vel);
	}

	gadgetTOC_del(&toc);
	gadgetHeader_del(&baseHeader);
	gadget_del(&gadget);
	cosmoModel_del(&model);
	grafic_del(&gvz);
	grafic_del(&gvy);
	grafic_del(&gvx);
} /* grafic2gadget_run */

/*--- Implementations of local functions --------------------------------*/
static void
local_getFactors(grafic_t     grafic,
                 uint32_t     np[3],
                 double       *dx,
                 double       *boxsize,
                 double       *vFact,
                 double       *aInit,
                 cosmoModel_t *model,
                 double       omegaBaryon0)
{
	double error, adot, growthVel;
	float  h0;

	grafic_getSize(grafic, np);
	h0       = grafic_getH0(grafic);
	*dx      = grafic_getDx(grafic);
	*aInit   = grafic_getAstart(grafic);
	*dx     *= 0.01 * h0;
	assert(np[0] == np[1] && np[0] == np[2]);
	*boxsize = *dx * np[0];

	*model   = cosmoModel_new();
	cosmoModel_setOmegaLambda0(*model, grafic_getOmegav(grafic));
	cosmoModel_setOmegaMatter0(*model, grafic_getOmegam(grafic));
	cosmoModel_setOmegaBaryon0(*model, omegaBaryon0);
	cosmoModel_setSmallH(*model, 0.01 * h0);

	adot      = cosmoModel_calcADot(*model, *aInit);
	growthVel = cosmoModel_calcDlnGrowthDlna(*model, *aInit, &error);
	*vFact    = 1. / (adot * 100. * growthVel);
}

static gadgetHeader_t
local_getBaseHeader(gadget_t     gadget,
                    double       boxsize,
                    double       aInit,
                    cosmoModel_t model,
                    uint32_t     np[3],
                    bool         doGas,
                    bool         useLongIDs,
                    double       posFactor)
{
	uint64_t       npall[6];
	double         massarr[6];
	double         omegaMatter0 = cosmoModel_getOmegaMatter0(model);
	int            numFiles     = gadget_getNumFiles(gadget);
	gadgetHeader_t header       = gadgetHeader_new();

	for (int i = 0; i < 6; i++) {
		npall[i]   = 0;
		massarr[i] = 0;
	}

	npall[1]    = np[0];
	npall[1]   *= np[1];
	npall[1]   *= np[2];
	massarr[1]  = boxsize * boxsize * boxsize * omegaMatter0 / (npall[1]);
	massarr[1] *= COSMO_RHO_CRIT0 * 1e-10;

	if (doGas) {
		double omegaBaryon0 = cosmoModel_getOmegaBaryon0(model);
		npall[0]    = npall[1];
		massarr[0]  = massarr[1] * omegaBaryon0 / omegaMatter0;
		massarr[1] -= massarr[0];
	}

	gadgetHeader_setMassArr(header, massarr);
	gadgetHeader_setTime(header, aInit);
	gadgetHeader_setRedshift(header, cosmo_a2z(aInit));
	gadgetHeader_setNall(header, npall);
	gadgetHeader_setNumFiles(header, numFiles);
	gadgetHeader_setBoxsize(header, boxsize * posFactor);
	gadgetHeader_setOmega0(header, cosmoModel_getOmegaMatter0(model));
	gadgetHeader_setOmegaLambda(header, cosmoModel_getOmegaLambda0(model));
	gadgetHeader_setHubbleParameter(header, cosmoModel_getSmallH(model));

	gadgetHeader_setUseLongIDs(header, useLongIDs);

	return header;
} /* local_getBaseHeader */

static gadgetTOC_t
local_getTOC(void)
{
	gadgetTOC_t toc;

	toc = gadgetTOC_new();
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_HEAD);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_POS_);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_VEL_);
	gadgetTOC_addEntryByType(toc, GADGETBLOCK_ID__);

	return toc;
}

static void
local_getSlabNumbers(int i,
                     int numFiles,
                     int range,
                     int *numSlabStart,
                     int *numSlabEnd,
                     int *numSlabs)
{
	int slabsPerTask = range / numFiles;
	int extra        = range % numFiles;

	if (i < extra) {
		*numSlabs     = slabsPerTask + 1;
		*numSlabStart = i * (*numSlabs);
		*numSlabEnd   = (i + 1) * (*numSlabs) - 1;
	} else {
		*numSlabs     = slabsPerTask;
		*numSlabStart = extra * (slabsPerTask + 1)
		                + (i - extra) * slabsPerTask;
		*numSlabEnd   = *numSlabStart + slabsPerTask - 1;
	}
}

static void
local_initposid(float    *pos,
                uint32_t *id,
                uint32_t np[3],
                int      zStart,
                int      zEnd,
                double   dx,
                bool     doGas)
{
	uint64_t numTotal  = np[0] * np[1] * np[2];
	uint64_t numOffset = np[0] * np[1] * (zEnd - zStart + 1);

#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (int k = zStart; k <= zEnd; k++) {
		for (uint32_t j = 0; j < np[1]; j++) {
			for (uint32_t i = 0; i < np[0]; i++) {
				size_t idx = i + j * np[0] + (k - zStart) * np[1] * np[0];
				id[idx]          = i + j * np[0] + k * np[1] * np[0];
				pos[idx * 3]     = (float)((i + .5) * dx);
				pos[idx * 3 + 1] = (float)((j + .5) * dx);
				pos[idx * 3 + 2] = (float)((k + .5) * dx);
				if (doGas) {
					double gasPosOffset = .25 * dx;
					id[idx + numOffset]              = id[idx];
					id[idx]                         += (uint32_t)numTotal;
					pos[numOffset * 3 + idx * 3]     = pos[idx * 3];
					pos[numOffset * 3 + idx * 3 + 1] = pos[idx * 3 + 1];
					pos[numOffset * 3 + idx * 3 + 2] = pos[idx * 3 + 2];
					pos[idx * 3]                    += gasPosOffset;
					pos[idx * 3 + 1]                += gasPosOffset;
					pos[idx * 3 + 2]                += gasPosOffset;
				}
			}
		}
	}
}

static void
local_initposidLong(float    *pos,
                    uint64_t *id,
                    uint32_t np[3],
                    int      zStart,
                    int      zEnd,
                    double   dx,
                    bool     doGas)
{
	uint64_t numTotal  = np[0] * np[1] * np[2];
	uint64_t numOffset = np[0] * np[1] * (zEnd - zStart + 1);

#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (int64_t k = zStart; k <= zEnd; k++) {
		for (uint64_t j = 0; j < np[1]; j++) {
			for (uint64_t i = 0; i < np[0]; i++) {
				size_t idx = i + j * np[0] + (k - zStart) * np[1] * np[0];
				id[idx]          = i + j * np[0] + k * np[1] * np[0];
				pos[idx * 3]     = (float)((i + .5) * dx);
				pos[idx * 3 + 1] = (float)((j + .5) * dx);
				pos[idx * 3 + 2] = (float)((k + .5) * dx);
				if (doGas) {
					double gasOffset = .25 * dx;
					id[idx + numOffset]             = id[idx];
					id[idx]                        += numTotal;
					pos[numTotal * 3 + idx * 3]     = pos[idx * 3];
					pos[numTotal * 3 + idx * 3 + 1] = pos[idx * 3 + 1];
					pos[numTotal * 3 + idx * 3 + 2] = pos[idx * 3 + 2];
					pos[idx * 3]                   += gasOffset;
					pos[idx * 3 + 1]               += gasOffset;
					pos[idx * 3 + 2]               += gasOffset;
				}
			}
		}
	}
}

static void
local_vel2pos(float    *vel,
              float    *pos,
              uint64_t num,
              double   boxsize,
              double   vFact,
              double   posFactor)
{
#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (uint64_t i = 0; i < num; i++) {
		pos[i * 3]     += (float)boxsize;
		pos[i * 3 + 1] += (float)boxsize;
		pos[i * 3 + 2] += (float)boxsize;
		pos[i * 3]      = (float)(fmod(pos[i * 3] + vFact * vel[i * 3],
		                               boxsize) * posFactor);
		pos[i * 3 + 1]  = (float)(fmod(pos[i * 3 + 1] + vFact * vel[i * 3 + 1],
		                               boxsize) * posFactor);
		pos[i * 3 + 2]  = (float)(fmod(pos[i * 3 + 2] + vFact * vel[i * 3 + 2],
		                               boxsize) * posFactor);
	}
}

static void
local_convertVel(float *vel, uint64_t num, double aInit, double velFactor)
{
	double fac = 1. / (sqrt(aInit)) * velFactor;

#ifdef _OPENMP
#  pragma omp parallel for
#endif
	for (uint64_t i = 0; i < num; i++) {
		vel[i * 3]     *= (float)fac;
		vel[i * 3 + 1] *= (float)fac;
		vel[i * 3 + 2] *= (float)fac;
	}
}

static void
local_checkForIDOverflow(uint32_t np[3], bool useLongIDs, bool doGas)
{
	uint64_t numParticles;

	numParticles  = np[0];
	numParticles *= np[1];
	numParticles *= np[2];

	if (doGas)
		numParticles *= 2;

	if ((numParticles > UINT32_MAX) && !useLongIDs) {
		fprintf(stderr, "WARNING:  IDs are overflowing!\n");
	}
}
