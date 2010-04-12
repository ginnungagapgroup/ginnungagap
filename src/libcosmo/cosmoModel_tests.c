// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoModel.h"
#include "cosmoModel_adt.h"
#include "cosmoModel_tests.h"
#include "cosmoFunc.h"
#include "../libutil/xmem.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#ifdef WITH_PROC_DIR
#  include <sys/types.h>
#  include <unistd.h>
#endif


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
cosmoModel_newFromFile_test(void)
{
	cosmoModel_t model;
	printf("Testing %s... ", __func__);
	model = cosmoModel_newFromFile("tests/model_EdS.dat");
	if (islessgreater(model->omegaRad0, 0.0))
		return false;

	if (islessgreater(model->omegaLambda0, 0.0))
		return false;

	if (islessgreater(model->omegaMatter0, 1.0))
		return false;

	if (islessgreater(model->omegaBaryon0, 0.0))
		return false;

	if (islessgreater(model->hubble, 0.5))
		return false;

	if (islessgreater(model->sigma8, 1.0))
		return false;

	if (islessgreater(model->ns, 1.0))
		return false;

	cosmoModel_del(&model);
	return true;
}

extern bool
cosmoModel_del_test(void)
{
	cosmoModel_t model;
#ifdef WITH_PROC_DIR
	char         buf[30];
	FILE         *pf;
	unsigned     sizeOld, sizeNew;
#endif

	printf("Testing %s... ", __func__);
#ifdef WITH_PROC_DIR
	snprintf(buf, 30, "/proc/%u/statm", (unsigned)getpid());
	pf = fopen(buf, "r");
	fscanf(pf, "%u", &sizeOld);
	rewind(pf);
#endif
	model = cosmoModel_newFromFile("tests/model_EdS.dat");
	cosmoModel_del(&model);
#ifdef WITH_PROC_DIR
	fscanf(pf, "%u", &sizeNew);
	if (sizeOld != sizeNew)
		return false;
#endif

	if (model != NULL)
		return false;

	return true;
}

extern bool
cosmoModel_calcAgeFromExpansion_test(void)
{
	cosmoModel_t model;
	double       age, ageEdS, a;
	double       error;
	bool         hasSucceeded = true;

	printf("Testing %s... ", __func__);
	model = cosmoModel_newFromFile("tests/model_EdS.dat");
	a     = 0.001;
	for (int i = 0; i < 100; i++) {
		a     += 1. / 100.;
		age    = cosmoModel_calcAgeFromExpansion(model, a, &error);
		ageEdS = 2. / 3. * pow(a, 1.5);
		if (isgreater(age - ageEdS, error)
		    || isless(age - ageEdS, -error))
			hasSucceeded = false;
	}
	cosmoModel_del(&model);
	return hasSucceeded;
}

extern bool
cosmoModel_calcExpansionFromAge_test(void)
{
	cosmoModel_t model;
	double       t, expansion, expansionEdS;
	double       error;
	bool         hasSucceeded = true;

	printf("Testing %s... ", __func__);
	model = cosmoModel_newFromFile("tests/model_EdS.dat");
	t     = 0.001;
	for (int i = 0; i < 100; i++) {
		t           += 1. / 100.;
		expansion    = cosmoModel_calcExpansionFromAge(model, t, &error);
		expansionEdS = pow(1.5 * t, 2. / 3.);
		if (isgreater(expansion - expansionEdS, error)
		    || isless(expansion - expansionEdS, -error))
			hasSucceeded = false;
	}
	cosmoModel_del(&model);
	return hasSucceeded;
}

extern bool
cosmoModel_calcADot_test(void)
{
	cosmoModel_t model;
	double       adot, adotEdS, a, t;
	double       error;
	bool         hasSucceeded = true;

	printf("Testing %s... ", __func__);
	model = cosmoModel_newFromFile("tests/model_EdS.dat");
	for (int i = 0; i < 10; i++) {
		a       = 1.0 - i * 0.1;
		adot    = cosmoModel_calcADot(model, a);
		t       = cosmoModel_calcAgeFromExpansion(model, a, &error);
		// adot_EdS = 2/3 * (3/2)^(2/3) * t^(-1/3)
		adotEdS = 2. / 3. * 1.3103706971044482 * pow(t, -1. / 3.);
		if (isgreater(fabs(adot - adotEdS), 1e-12))
			hasSucceeded = false;
	}
	cosmoModel_del(&model);

	return hasSucceeded;
}

extern bool
cosmoModel_calcOmegas_test(void)
{
	cosmoModel_t model;
	bool         hasSucceeded = true;
	double       a, omegaRad, omegaMatter, omegaBaryon, omegaLambda, t;
	long double  sumOmegas0;
	double       barFrac;

	printf("Testing %s... ", __func__);
	model      = cosmoModel_newFromFile("tests/model_wmap.dat");
	sumOmegas0 = model->omegaRad0 + model->omegaMatter0
	             + model->omegaLambda0;
	barFrac    = model->omegaBaryon0 / model->omegaMatter0;
	for (int i = 0; i < 10; i++) {
		a           = exp(log(1e-6) + ((log(1.) - log(1e-6)) / 9.) * i);
		omegaRad    = cosmoModel_calcOmegaRad(model, a);
		omegaMatter = cosmoModel_calcOmegaMatter(model, a);
		omegaBaryon = cosmoModel_calcOmegaBaryon(model, a);
		omegaLambda = cosmoModel_calcOmegaLambda(model, a);
		if (isgreater(fabs(sumOmegas0 - omegaRad - omegaMatter
		                   - omegaLambda), 1e-4))
			hasSucceeded = false;
		if (isgreater(fabs(barFrac - omegaBaryon / omegaMatter), 1e-10))
			hasSucceeded = false;
	}
	cosmoModel_del(&model);
	return hasSucceeded;
}

extern bool
cosmoModel_calcGrowth_test(void)
{
	cosmoModel_t model;
	bool         hasSucceeded = true;
	double       a, error;
	double D, DEdS;

	printf("Testing %s... ", __func__);
	model = cosmoModel_newFromFile("tests/model_EdS.dat");
	for (int i=0; i<10; i++) {
		a = 1.0-i*0.1;
		D = cosmoModel_calcGrowth(model, a, &error);
		DEdS = a;
		if (isgreater(fabs(D-DEdS),1e-10))
			hasSucceeded = false;
	}
	cosmoModel_del(&model);
	return hasSucceeded;
}

/*--- Implementations of local functions --------------------------------*/
