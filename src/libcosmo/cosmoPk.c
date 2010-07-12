// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoPk.h"
#include "cosmoFunc.h"
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"
#include "../libutil/diediedie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <assert.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_integration.h>


/*--- Implemention of the ADT structure ---------------------------------*/
#include "cosmoPk_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_MAX_FORCESIGMA8_ITERATIONS 42


/*--- Prototypes of local functions -------------------------------------*/
static cosmoPk_t
local_new(void);

static void
local_getMem(cosmoPk_t pk, uint32_t numPoints);

static void
local_doInterpolation(cosmoPk_t pk);

static cosmoPk_t
local_constructPkFromModel(parse_ini_t ini, const char *sectionName);

static void
local_initKs(cosmoPk_t pk, double kmin, double kmax);

static void
local_calcPkFromTransferFunction(cosmoPk_t          pk,
                                 cosmoTF_t          transferFunctionType,
                                 const cosmoModel_t model);


/*--- Local structures and typedefs -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern cosmoPk_t
cosmoPk_newFromFile(const char *fname)
{
	cosmoPk_t pk;
	FILE      *f;

	assert(fname != NULL);

	pk = local_new();
	f  = xfopen(fname, "r");
	if (fscanf(f, "# %" SCNu32 " %lf %lf \n",
	           &(pk->numPoints),
	           &(pk->slopeBeforeKmin),
	           &(pk->slopeBeyondKmax)) != 3) {
		diediedie(EXIT_FAILURE);
	}
	local_getMem(pk, pk->numPoints);
	for (uint32_t i = 0; i < pk->numPoints; i++) {
		if (fscanf(f, " %lf %lf \n", pk->k + i, pk->P + i) != 2) {
			diediedie(EXIT_FAILURE);
		}
	}
	xfclose(&f);
	local_doInterpolation(pk);

	return pk;
}

extern cosmoPk_t
cosmoPk_newFromIni(parse_ini_t ini, const char *sectionName)
{
	cosmoPk_t pk;
	bool      hasFName;
	char      *fileNamePk = NULL;

	hasFName = parse_ini_get_string(ini, "powerSpectrumFileName",
	                                sectionName, &fileNamePk);
	if (hasFName) {
		pk = cosmoPk_newFromFile(fileNamePk);
		xfree(fileNamePk);
	} else {
		pk = local_constructPkFromModel(ini, sectionName);
	}

	return pk;
}

extern cosmoPk_t
cosmoPk_newFromModel(const cosmoModel_t model,
                     double             kmin,
                     double             kmax,
                     uint32_t           numPoints,
                     cosmoTF_t          transferFunctionType)
{
	cosmoPk_t pk;

	pk                  = local_new();
	pk->numPoints       = numPoints;
	pk->slopeBeforeKmin = cosmoModel_getNs(model);
	pk->slopeBeyondKmax = -3;
	local_getMem(pk, pk->numPoints);
	local_initKs(pk, kmin, kmax);

	local_calcPkFromTransferFunction(pk, transferFunctionType, model);
	local_doInterpolation(pk);

	return pk;
}

extern cosmoPk_t
cosmoPk_newFromArrays(uint32_t     numPoints,
                      const double *k,
                      const double *P,
                      double       slopeBeforeKmin,
                      double       slopeBeyondKmax)
{
	cosmoPk_t pk;
	assert(k != NULL && P != NULL);

	pk = local_new();
	local_getMem(pk, numPoints);
	memcpy(pk->k, k, sizeof(double) * numPoints);
	memcpy(pk->P, P, sizeof(double) * numPoints);
	pk->slopeBeforeKmin = slopeBeforeKmin;
	pk->slopeBeyondKmax = slopeBeyondKmax;
	local_doInterpolation(pk);

	return pk;
}

extern void
cosmoPk_del(cosmoPk_t *pk)
{
	assert(pk != NULL && *pk != NULL);

	if ((*pk)->k != NULL)
		xfree((*pk)->k);
	if ((*pk)->acc != NULL)
		gsl_interp_accel_free((*pk)->acc);
	if ((*pk)->spline != NULL)
		gsl_spline_free((*pk)->spline);
	xfree(*pk);
	*pk = NULL;
}

extern void
cosmoPk_dumpToFile(cosmoPk_t pk, const char *fname, uint32_t numSubSample)
{
	FILE   *f;
	assert(pk != NULL && fname != NULL);
	double k, P;

	numSubSample = (numSubSample == 0) ? 1 : numSubSample;

	f            = xfopen(fname, "w");
	for (uint32_t i = 0; i < pk->numPoints - 1; i++) {
		for (uint32_t j = 0; j < numSubSample; j++) {
			k = pk->k[i] + j * (pk->k[i + 1] - pk->k[i]) / numSubSample;
			P = cosmoPk_eval(pk, k);
			fprintf(f, "%15.12g\t%15.12g\n", k, P);
		}
	}
	xfclose(&f);
}

extern double
cosmoPk_eval(cosmoPk_t pk, double k)
{
	assert(pk != NULL);
	assert(isgreater(k, 0.0));

	if (isless(k, pk->k[0]))
		return pk->P[0] * pow(k / (pk->k[0]), pk->slopeBeforeKmin);

	if (isgreater(k, pk->k[pk->numPoints - 1]))
		return pk->P[0] * pow(k / (pk->k[pk->numPoints - 1]),
		                      pk->slopeBeyondKmax);

	return gsl_spline_eval(pk->spline, k, pk->acc);
}

extern double
cosmoPk_evalGSL(double k, void *param)
{
	assert(param != NULL);
	assert(isgreater(k, 0.0));
	return cosmoPk_eval((cosmoPk_t)param, k);
}

extern double
cosmoPk_calcMomentFiltered(cosmoPk_t pk,
                           uint32_t moment,
                           double (*windowFunc
                                   )(double, void *),
                           void *paramWindowFunc,
                           double kmin,
                           double kmax,
                           double *error)
{
	double                      sigmaSqr;
	double                      exponentK = (2. * moment + 2.);
	cosmoFunc_product3_struct_t param;
	gsl_integration_workspace   *w;
	gsl_function                F;

	assert(pk != NULL && error != NULL && windowFunc != NULL);

	param.f1      = &cosmoPk_evalGSL;
	param.paramF1 = (void *)pk;
	param.f2      = windowFunc;
	param.paramF2 = paramWindowFunc;
	param.f3      = &cosmoFunc_xPowerY;
	param.paramF3 = (void *)&exponentK;
	F.function    = &cosmoFunc_product3;
	F.params      = (void *)&param;

	w             = gsl_integration_workspace_alloc(1000);

	gsl_integration_qags(&F, kmin, kmax, 0, 9e-9, 1000,
	                     w, &sigmaSqr, error);

	gsl_integration_workspace_free(w);

	return 1. / (TWOPI * PI) * sigmaSqr;
}

extern double
cosmoPk_calcSigma8(cosmoPk_t pk,
                   double    kmin,
                   double    kmax,
                   double    *error)
{
	double scale = 8.0;
	double sigma8Sqr;

	assert(pk != NULL && isgreater(kmax, kmin) && isgreater(kmin, 0.0)
	       && error != NULL);

	sigma8Sqr = cosmoPk_calcMomentFiltered(pk, UINT32_C(0),
	                                       &cosmoFunc_tophatSqr,
	                                       &scale, kmin, kmax, error);
	return sqrt(sigma8Sqr);
}

extern void
cosmoPk_scale(cosmoPk_t pk, double factor)
{
	assert(pk != NULL);
	assert(isgreater(factor, 0.0));

	for (uint32_t i = 0; i < pk->numPoints; i++)
		pk->P[i] *= factor;
	local_doInterpolation(pk);
}

extern double
cosmoPk_forceSigma8(cosmoPk_t pk,
                    double    sigma8,
                    double    kmin,
                    double    kmax,
                    double    *error)
{
	double sigma8Actual;
	double sigma8First;
	int    numIter    = 0;
	int    numIterMax = LOCAL_MAX_FORCESIGMA8_ITERATIONS;

	assert(pk != NULL);
	assert(isgreater(sigma8, 0.0));
	assert(isgreater(kmin, 0.0));
	assert(isgreater(kmax, kmin));

	sigma8Actual = cosmoPk_calcSigma8(pk, kmin, kmax, error);
	sigma8First  = sigma8Actual;
	do {
		cosmoPk_scale(pk, sigma8 / sigma8Actual);
		sigma8Actual = cosmoPk_calcSigma8(pk, kmin, kmax, error);
		*error       = fabs(1. - sigma8 / sigma8Actual);
		numIter++;
	} while (numIter < numIterMax && isgreater(*error, 1e-10));
	if (numIter >= numIterMax)
		fprintf(stderr, "Exhausted iterations in %s: error %15.13e\n",
		        __func__, *error);


	return sigma8First / sigma8;
}

/*--- Implementations of local functions --------------------------------*/
static cosmoPk_t
local_new(void)
{
	cosmoPk_t pk = xmalloc(sizeof(struct cosmoPk_struct));

	pk->numPoints       = UINT32_C(0);
	pk->k               = NULL;
	pk->P               = NULL;
	pk->slopeBeyondKmax = 1e10;
	pk->slopeBeforeKmin = 1e10;
	pk->acc             = NULL;
	pk->spline          = NULL;

	return pk;
}

static void
local_getMem(cosmoPk_t pk, uint32_t numPoints)
{
	if (pk->k != NULL)
		xfree(pk->k);
	pk->k         = xmalloc(sizeof(double) * numPoints * 2);
	pk->P         = pk->k + numPoints;
	pk->numPoints = numPoints;
}

static void
local_doInterpolation(cosmoPk_t pk)
{
	pk->acc    = gsl_interp_accel_alloc();
	pk->spline = gsl_spline_alloc(gsl_interp_cspline,
	                              (int)(pk->numPoints));
	gsl_spline_init(pk->spline, pk->k, pk->P, (int)(pk->numPoints));
}

static cosmoPk_t
local_constructPkFromModel(parse_ini_t ini, const char *sectionName)
{
	cosmoPk_t    pk;
	cosmoModel_t model = cosmoModel_newFromIni(ini, sectionName);
	double       kmin, kmax;
	uint32_t     numPoints;

	getFromIni(&kmin, parse_ini_get_double, ini,
	           "powerSpectrumKmin", sectionName);
	getFromIni(&kmax, parse_ini_get_double, ini,
	           "powerSpectrumKmax", sectionName);
	getFromIni(&numPoints, parse_ini_get_uint32, ini,
	           "powerSpectrumNumPoints", sectionName);

	pk = cosmoPk_newFromModel(model, kmin, kmax, numPoints,
	                          cosmoTF_getTypeFromIni(ini, sectionName));
	cosmoModel_del(&model);

	return pk;
}

static void
local_initKs(cosmoPk_t pk, double kmin, double kmax)
{
	pk->k[0]                 = kmin;
	pk->k[pk->numPoints - 1] = kmax;

	if (pk->numPoints > 1) {
		double logKmin = log(kmin);
		double logdk   = (log(kmax) - logKmin)
		                 / ((double)(pk->numPoints - 1));
		for (uint32_t i = 1; i < pk->numPoints - 1; i++)
			pk->k[i] = exp(logKmin + logdk * i);
	}
}

static void
local_calcPkFromTransferFunction(cosmoPk_t          pk,
                                 cosmoTF_t          transferFunctionType,
                                 const cosmoModel_t model)
{
	switch (transferFunctionType) {
	case COSMOTF_TYPE_ANATOLY2000:
	case COSMOTF_TYPE_EISENSTEINHU1998:
	default:
		cosmoTF_eisensteinHu1998(cosmoModel_getOmegaMatter0(model),
		                         cosmoModel_getOmegaBaryon0(model),
		                         cosmoModel_getSmallH(model),
		                         cosmoModel_getTempCMB(model),
		                         pk->numPoints, pk->k, pk->P);
		break;
	}

	for (uint32_t i = 0; i < pk->numPoints; i++) {
		pk->P[i] *= pk->P[i] * pow(pk->k[i], cosmoModel_getNs(model));
	}
}
