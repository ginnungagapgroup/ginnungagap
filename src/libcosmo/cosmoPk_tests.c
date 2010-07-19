// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoPk.h"
#include "cosmoPk_adt.h"
#include "cosmoPk_tests.h"
#include "cosmoFunc.h"
#include "cosmoModel.h"
#include "../libutil/xmem.h"
#include "../libutil/utilMath.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
cosmoPk_newFromFile_test(void)
{
	cosmoPk_t pk;
	bool      hasPassed = true;

	printf("Testing %s... ", __func__);

	pk = cosmoPk_newFromFile("tests/pk.dat");
	if (pk->numPoints != 100)
		hasPassed = false;
	if (islessgreater(pk->slopeBeyondKmax, -3.0))
		hasPassed = false;
	if (islessgreater(pk->slopeBeforeKmin, 1.0))
		hasPassed = false;
	if (islessgreater(pk->k[0], 0.000015))
		hasPassed = false;
	if (islessgreater(pk->P[0], 1.49999e-05))
		hasPassed = false;
	if (islessgreater(pk->k[99], 105.436996))
		hasPassed = false;
	if (islessgreater(pk->P[99], 4.21748e-10))
		hasPassed = false;

	cosmoPk_del(&pk);

	return hasPassed ? true : false;
}

extern bool
cosmoPk_newFromModel_test(void)
{
	cosmoPk_t    pk;
	cosmoModel_t model;
	bool         hasPassed = true;

	printf("Testing %s... ", __func__);
	model = cosmoModel_newFromFile("tests/model_wmap7.dat");

	pk    = cosmoPk_newFromModel(model, 1e-2, 1e1, 450,
	                             COSMOTF_TYPE_EISENSTEINHU1998);
	cosmoPk_dumpToFile(pk, "pk_from_tf.dat", 10);
	cosmoPk_del(&pk);

	cosmoModel_del(&model);

	return hasPassed ? true : false;
}

extern bool
cosmoPk_newFromArrays_test(void)
{
	cosmoPk_t pk;
	double    k[] = { 1.0, 2.0, 3.0};
	double    P[] = { 1.0, 5.0, 2.0};
	printf("Testing %s... ", __func__);
	pk = cosmoPk_newFromArrays(UINT32_C(3), k, P, 1.0, -3.0);

	if (pk->numPoints != UINT32_C(3))
		return false;

	if (islessgreater(pk->k[0], 1.0))
		return false;

	if (islessgreater(pk->P[0], 1.0))
		return false;

	if (islessgreater(pk->k[2], 3.0))
		return false;

	if (islessgreater(pk->P[2], 2.0))
		return false;

	if (islessgreater(pk->slopeBeforeKmin, 1.0))
		return false;

	if (islessgreater(pk->slopeBeyondKmax, -3.0))
		return false;

	cosmoPk_del(&pk);
	return true;
}

extern bool
cosmoPk_del_test(void)
{
	cosmoPk_t pk;

	printf("Testing %s... ", __func__);
	pk = cosmoPk_newFromFile("tests/pk.dat");
	cosmoPk_del(&pk);

	if (pk != NULL)
		return false;

	return true;
}

extern bool
cosmoPk_eval_test(void)
{
	cosmoPk_t pk;
	double    testVal;

	printf("Testing %s... ", __func__);
	pk      = cosmoPk_newFromFile("tests/pk.dat");
	testVal = cosmoPk_eval(pk, 0.005528);
	if (islessgreater(testVal, 0.00437133))
		return false;

	testVal = cosmoPk_eval(pk, 3.192540);
	if (islessgreater(testVal, 2.54019e-06))
		return false;

	testVal = cosmoPk_eval(pk, 55.826096);
	if (islessgreater(testVal, 1.39565e-09))
		return false;

	cosmoPk_del(&pk);
	return true;
}

extern bool
cosmoPk_calcMomentFiltered_test(void)
{
	cosmoPk_t pk;
	double    *k, *P;
	double    error, sigmaSqr, sigmaSqrTheo;
	double    exponent     = -1.0;
	bool      hasSucceeded = true;

	printf("Testing %s... ", __func__);
	k = xmalloc(sizeof(double) * 100 * 2);
	P = k + 100;
	for (int i = 0; i < 100; i++) {
		k[i] = exp(log(0.1) + i * 0.1);
		P[i] = 1. / k[i];
	}
	pk           = cosmoPk_newFromArrays(100, k, P, -1.0, -1.0);

	sigmaSqrTheo = 1. / ((2. * 0. + 1.) * 2 * POW2(M_PI))
	               * (pow(k[99], 2. * 0. + 1.)
	                  - pow(k[10], 2. * 0. + 1.));
	sigmaSqr = cosmoPk_calcMomentFiltered(pk, UINT32_C(0),
	                                      &cosmoFunc_xPowerY,
	                                      &exponent,
	                                      k[10], k[99],
	                                      &error);
	if (isgreater(fabs(sigmaSqr / sigmaSqrTheo - 1.0), 4e-5))
		hasSucceeded = false;
	sigmaSqrTheo = 1. / ((2. * 1. + 1.) * 2 * POW2(M_PI))
	               * (pow(k[99], 2. * 1. + 1.)
	                  - pow(k[10], 2. * 1. + 1.));
	sigmaSqr = cosmoPk_calcMomentFiltered(pk, UINT32_C(1),
	                                      &cosmoFunc_xPowerY,
	                                      &exponent,
	                                      k[10], k[99],
	                                      &error);
	if (isgreater(fabs(sigmaSqr / sigmaSqrTheo - 1.0), 2e-4))
		hasSucceeded = false;

	xfree(k);
	cosmoPk_del(&pk);
	return hasSucceeded;
} /* cosmoPk_calcMomentFiltered_test */

extern bool
cosmoPk_calcSigma8_test(void)
{
	cosmoPk_t    pk;
	cosmoModel_t model;
	double       sigma8, error;
	bool         hasPassed      = true;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	model = cosmoModel_newFromFile("tests/model_wmap7.dat");
	pk    = cosmoPk_newFromModel(model, 1e-2, 1e1, 450,
	                             COSMOTF_TYPE_EISENSTEINHU1998);

	sigma8 = cosmoPk_calcSigma8(pk, 2e-2, 9e0, &error);
	if (isgreater(fabs(sigma8 - 4.0239589333153e-04), sqrt(error)))
		hasPassed = false;

	cosmoPk_del(&pk);
	cosmoModel_del(&model);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
cosmoPk_scale_test(void)
{
	cosmoPk_t    pk;
	cosmoModel_t model;
	double       p0[3], p1[3];
	bool         hasPassed      = true;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	model = cosmoModel_newFromFile("tests/model_wmap7.dat");
	pk    = cosmoPk_newFromModel(model, 1e-2, 1e1, 450,
	                             COSMOTF_TYPE_EISENSTEINHU1998);

	p0[0] = cosmoPk_eval(pk, 0.05);
	p0[1] = cosmoPk_eval(pk, 0.5);
	p0[2] = cosmoPk_eval(pk, 5.);
	cosmoPk_scale(pk, M_PI);
	p1[0] = cosmoPk_eval(pk, 0.05);
	p1[1] = cosmoPk_eval(pk, 0.5);
	p1[2] = cosmoPk_eval(pk, 5.);
	if (isgreater(fabs(p1[0] / p0[0] - M_PI), 1e-15))
		hasPassed = false;
	if (isgreater(fabs(p1[1] / p0[1] - M_PI), 1e-15))
		hasPassed = false;
	if (isgreater(fabs(p1[2] / p0[2] - M_PI), 1e-15))
		hasPassed = false;

	cosmoPk_del(&pk);
	cosmoModel_del(&model);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* cosmoPk_scale_test */

extern bool
cosmoPk_forceSigma8_test(void)
{
	cosmoPk_t    pk;
	cosmoModel_t model;
	bool         hasPassed = true;
	double       factor, error;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	model = cosmoModel_newFromFile("tests/model_wmap7.dat");
	pk    = cosmoPk_newFromModel(model, 1e-2, 1e1, 450,
	                             COSMOTF_TYPE_EISENSTEINHU1998);

	factor = cosmoPk_forceSigma8(pk, 0.8, 5e-2, 5, &error);
	if (isgreater(error, 1e-10))
		hasPassed = false;

	cosmoPk_del(&pk);
	cosmoModel_del(&model);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
