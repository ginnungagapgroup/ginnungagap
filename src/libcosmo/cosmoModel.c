// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


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


/*--- Implemention of the ADT structure ---------------------------------*/
#include "cosmoModel_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
static cosmoModel_t
local_new(void);


/*--- Implementations of exported functios ------------------------------*/
extern cosmoModel_t
cosmoModel_newFromFile(const char *fname)
{
	cosmoModel_t model;
	FILE         *f;

	f     = xfopen(fname, "r");
	model = local_new();
	fscanf(f, "omegaRad0 = %lf \n", &(model->omegaRad0));
	fscanf(f, "omegaLambda0 = %lf \n", &(model->omegaLambda0));
	fscanf(f, "omegaMatter0 = %lf \n", &(model->omegaMatter0));
	fscanf(f, "omegaBaryon0 = %lf \n", &(model->omegaBaryon0));
	fscanf(f, "hubble = %lf \n", &(model->hubble));
	fscanf(f, "sigma8 = %lf \n", &(model->sigma8));
	fscanf(f, "ns = %lf \n", &(model->ns));
	xfclose(&f);

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

/*--- Implementations of local functions --------------------------------*/
static cosmoModel_t
local_new(void)
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

	return model;
}
