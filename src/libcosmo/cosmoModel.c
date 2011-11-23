// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libcosmo/cosmoModel.c
 * @ingroup  libcosmoModel
 * @brief  Provides the implementation of the cosmological model object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoModel.h"
#include "cosmoFunc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <gsl/gsl_integration.h>
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"
#include "../libutil/diediedie.h"


/*--- Implemention of the ADT structure ---------------------------------*/
#include "cosmoModel_adt.h"


/*--- Local defines -----------------------------------------------------*/

/** @brief  The default value of OmegaRad0. */
#define LOCAL_DEFAULT_OMEGARAD0 0.0
/** @brief  The default value of OmegaBaryon0. */
#define LOCAL_DEFAULT_OMEGABARYON0 0.0
/** @brief  The default value of the temperature of the CMB in K. */
#define LOCAL_DEFAULT_TEMPCMB 2.75
/** @brief  The default value of the slope of the primordial power spectrum. */
#define LOCAL_DEFAULT_NS 1.0


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern cosmoModel_t
cosmoModel_new(void)
{
	cosmoModel_t model;

	model               = xmalloc(sizeof(struct cosmoModel_struct));
	model->omegaRad0    = 0.0;
	model->omegaLambda0 = 0.0;
	model->omegaMatter0 = 0.0;
	model->omegaBaryon0 = 0.0;
	model->hubble       = 0.0;
	model->sigma8       = 0.0;
	model->ns           = 0.0;
	model->tempCMB      = 0.0;

	return model;
}

#define CHECK(got, expected)   \
	{                          \
		if (got != expected) { \
			diediedie(2312);   \
		}                      \
	}
extern cosmoModel_t
cosmoModel_newFromFile(const char *fname)
{
	cosmoModel_t model;
	FILE         *f;
	int          rtn;

	f     = xfopen(fname, "r");
	model = cosmoModel_new();
	rtn   = fscanf(f, "omegaRad0 = %lf \n", &(model->omegaRad0));
	CHECK(rtn, 1);
	rtn   = fscanf(f, "omegaLambda0 = %lf \n", &(model->omegaLambda0));
	CHECK(rtn, 1);
	rtn   = fscanf(f, "omegaMatter0 = %lf \n", &(model->omegaMatter0));
	CHECK(rtn, 1);
	rtn   = fscanf(f, "omegaBaryon0 = %lf \n", &(model->omegaBaryon0));
	CHECK(rtn, 1);
	rtn   = fscanf(f, "hubble = %lf \n", &(model->hubble));
	CHECK(rtn, 1);
	rtn   = fscanf(f, "sigma8 = %lf \n", &(model->sigma8));
	CHECK(rtn, 1);
	rtn   = fscanf(f, "ns = %lf \n", &(model->ns));
	CHECK(rtn, 1);
	rtn   = fscanf(f, "tempCMB = %lf \n", &(model->tempCMB));
	CHECK(rtn, 1);
	xfclose(&f);

	return model;
}

#undef CHECK
extern cosmoModel_t
cosmoModel_newFromIni(parse_ini_t ini, const char *sectionName)
{
	cosmoModel_t model;

	model = cosmoModel_new();
	if (!parse_ini_get_double(ini, "modelOmegaRad0", sectionName,
	                          &(model->omegaRad0)))
		model->omegaRad0 = LOCAL_DEFAULT_OMEGARAD0;
	getFromIni(&(model->omegaLambda0), parse_ini_get_double,
	           ini, "modelOmegaLambda0", sectionName);
	getFromIni(&(model->omegaMatter0), parse_ini_get_double,
	           ini, "modelOmegaMatter0", sectionName);
	if (!parse_ini_get_double(ini, "modelOmegaBaryon0", sectionName,
	                          &(model->omegaBaryon0)))
		model->omegaBaryon0 = LOCAL_DEFAULT_OMEGABARYON0;
	getFromIni(&(model->hubble), parse_ini_get_double,
	           ini, "modelHubble", sectionName);
	getFromIni(&(model->sigma8), parse_ini_get_double,
	           ini, "modelSigma8", sectionName);
	if (!parse_ini_get_double(ini, "modelNs", sectionName,
	                          &(model->ns)))
		model->ns = LOCAL_DEFAULT_NS;
	if (!parse_ini_get_double(ini, "modelTempCMB", sectionName,
	                          &(model->tempCMB)))
		model->tempCMB = LOCAL_DEFAULT_TEMPCMB;

	return model;
}

extern void
cosmoModel_del(cosmoModel_t *model)
{
	assert(model != NULL && *model != NULL);
	xfree(*model);
	*model = NULL;
}

extern double
cosmoModel_getOmegaRad0(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->omegaRad0;
}

extern double
cosmoModel_getOmegaLambda0(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->omegaLambda0;
}

extern double
cosmoModel_getOmegaMatter0(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->omegaMatter0;
}

extern double
cosmoModel_getOmegaBaryon0(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->omegaBaryon0;
}

extern double
cosmoModel_getSmallH(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->hubble;
}

extern double
cosmoModel_getSigma8(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->sigma8;
}

extern double
cosmoModel_getNs(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->ns;
}

extern double
cosmoModel_getTempCMB(const cosmoModel_t model)
{
	assert(model != NULL);

	return model->tempCMB;
}

extern void
cosmoModel_setOmegaRad0(cosmoModel_t model, const double omegaRad0)
{
	assert(model != NULL);

	model->omegaRad0 = omegaRad0;
}

extern void
cosmoModel_setOmegaLambda0(cosmoModel_t model, const double omegaLambda0)
{
	assert(model != NULL);

	model->omegaLambda0 = omegaLambda0;
}

extern void
cosmoModel_setOmegaMatter0(cosmoModel_t model, const double omegaMatter0)
{
	assert(model != NULL);

	model->omegaMatter0 = omegaMatter0;
}

extern void
cosmoModel_setOmegaBaryon0(cosmoModel_t model, const double omegaBaryon0)
{
	assert(model != NULL);

	model->omegaBaryon0 = omegaBaryon0;
}

extern void
cosmoModel_setSmallH(cosmoModel_t model, const double hubble)
{
	assert(model != NULL);

	model->hubble = hubble;
}

extern void
cosmoModel_setSigma8(cosmoModel_t model, const double sigma8)
{
	assert(model != NULL);

	model->sigma8 = sigma8;
}

extern void
cosmoModel_setNs(cosmoModel_t model, const double ns)
{
	assert(model != NULL);

	model->ns = ns;
}

extern void
cosmoModel_setTempCMB(cosmoModel_t model, const double tempCMB)
{
	assert(model != NULL);

	model->tempCMB = tempCMB;
}

extern double
cosmoModel_calcAgeFromExpansion(cosmoModel_t model,
                                double       a,
                                double       *error)
{
	double                    age;
	cosmoFunc_dtda_struct_t   param;
	gsl_integration_workspace *w;
	gsl_function              F;

	assert(model != NULL && isgreater(a, 0.0));

	param.omegaRad0    = model->omegaRad0;
	param.omegaMatter0 = model->omegaMatter0;
	param.omegaLambda0 = model->omegaLambda0;
	F.function         = &cosmoFunc_dtda;
	F.params           = &param;
	w                  = gsl_integration_workspace_alloc(1000);
	gsl_integration_qags(&F, 0.0, a, 0, 1e-8, 1000, w, &age, error);
	gsl_integration_workspace_free(w);

	return age;
}

extern double
cosmoModel_calcExpansionFromAge(cosmoModel_t model,
                                double       t,
                                double       *error)
{
	double aUpper = 2.0;
	double aLower = 0.0;
	double expansion;
	double tGuess;
	bool   converged = false;

	assert(model != NULL && isgreater(t, 0.0));
	assert(isless(t, cosmoModel_calcAgeFromExpansion(model, aUpper, error)));

	while (!converged) {
		expansion = 0.5 * (aLower + aUpper);
		tGuess    = cosmoModel_calcAgeFromExpansion(model, expansion, error);
		if (isless(fabs(aUpper - aLower), 1e-10)) {
			converged = true;
			*error    = fabs(aUpper - aLower) + fabs(*error);
			continue;
		}
		if (isless(tGuess, t))
			aLower = expansion;
		else
			aUpper = expansion;
	}

	return expansion;
}

extern double
cosmoModel_calcADot(cosmoModel_t model, double a)
{
	cosmoFunc_dtda_struct_t param;
	param.omegaRad0    = model->omegaRad0;
	param.omegaMatter0 = model->omegaMatter0;
	param.omegaLambda0 = model->omegaLambda0;
	return 1. / (cosmoFunc_dtda(a, &param));
}

extern double
cosmoModel_calcHubble(cosmoModel_t model, double a)
{
	double aInv    = 1. / a;
	double aSqrInv = aInv * aInv;
	assert(model != NULL && isgreater(a, 0.0));
	return sqrt(model->omegaRad0 * (aSqrInv * (aSqrInv - 1.0))
	            + model->omegaMatter0 * (aSqrInv * (aInv - 1.0))
	            + model->omegaLambda0 * (1. - aSqrInv)
	            + aSqrInv);
}

extern double
cosmoModel_calcOmegaRad(cosmoModel_t model, double a)
{
	double aSqr = a * a;
	assert(model != NULL && isgreater(a, 0.0));
	return model->omegaRad0
	       / (model->omegaRad0 * (1. - aSqr)
	          + model->omegaMatter0 * (a - aSqr)
	          + model->omegaLambda0 * (aSqr * aSqr - aSqr)
	          + aSqr);
}

extern double
cosmoModel_calcOmegaMatter(cosmoModel_t model, double a)
{
	assert(model != NULL && isgreater(a, 0.0));
	return model->omegaMatter0
	       / (model->omegaRad0 * (1. / a - a)
	          + model->omegaMatter0 * (1.0 - a)
	          + model->omegaLambda0 * (a * a * a - a)
	          + a);
}

extern double
cosmoModel_calcOmegaBaryon(cosmoModel_t model, double a)
{
	assert(model != NULL && isgreater(a, 0.0));
	return cosmoModel_calcOmegaMatter(model, a)
	       * model->omegaBaryon0 / model->omegaMatter0;
}

extern double
cosmoModel_calcOmegaLambda(cosmoModel_t model, double a)
{
	double aSqr = a * a;
	assert(model != NULL && isgreater(a, 0.0));
	return model->omegaLambda0 * aSqr * aSqr
	       / (model->omegaRad0 * (1. - aSqr)
	          + model->omegaMatter0 * (a - aSqr)
	          + model->omegaLambda0 * (aSqr * aSqr - aSqr)
	          + aSqr);
}

extern double
cosmoModel_calcGrowth(cosmoModel_t model, double a, double *error)
{
	double                    tmp1;
	cosmoFunc_dtda_struct_t   param;
	gsl_integration_workspace *w;
	gsl_function              F;

	assert(model != NULL && isgreater(a, 0.0) && error != NULL);

	param.omegaRad0    = model->omegaRad0;
	param.omegaMatter0 = model->omegaMatter0;
	param.omegaLambda0 = model->omegaLambda0;
	F.function         = &cosmoFunc_dtdaCube;
	F.params           = &param;
	w                  = gsl_integration_workspace_alloc(1000);
	gsl_integration_qags(&F, 0.0, a, 0, 1e-8, 1000, w, &tmp1, error);
	gsl_integration_workspace_free(w);

	return 2.5 * (model->omegaMatter0)
	       * cosmoModel_calcADot(model, a)
	       * tmp1 / a;
}

extern double
cosmoModel_calcDlnGrowthDlna(cosmoModel_t model, double a, double *error)
{
	double growth;
	double fupper;
	double flower;
	double ainv;
	double tmp;

	assert(model != NULL);
	assert(error != NULL);
	assert(isgreater(a, 0.0));

	growth = cosmoModel_calcGrowth(model, a, error);
	ainv   = 1. / a;

	tmp    = 1. - (model->omegaRad0) - (model->omegaMatter0)
	         - (model->omegaLambda0);
	fupper = 2.5 * (model->omegaMatter0) / growth
	         - 2. * (model->omegaRad0) * ainv * ainv
	         - 1.5 *  (model->omegaMatter0) * ainv
	         - tmp;
	flower = (model->omegaRad0) * ainv * ainv
	         + (model->omegaMatter0) * ainv
	         + (model->omegaLambda0) * a * a
	         + tmp;

	return fupper / flower;
}

extern double
cosmoModel_calcDlnGrowthDlna2lpt(cosmoModel_t model,
                                 double       a,
                                 double       *error)
{
	double omegaM;

	assert(model != NULL);
	assert(error != NULL);

	assert(isgreater(model->omegaLambda0, 0.0));

	omegaM = cosmoModel_calcOmegaMatter(model, a);
	assert(isgreater(omegaM, 0.1) && isless(omegaM, 1.0));
	*error = 0.0;

	return 2. * pow(omegaM, 6. / 11.);
}


/*--- Implementations of local functions --------------------------------*/
