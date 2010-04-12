// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoPk.h"
#include "cosmoPk_adt.h"
#include "cosmoPk_tests.h"
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
cosmoPk_newFromFile_test(void)
{
	cosmoPk_t pk;
	printf("Testing %s... ", __func__);
	pk = cosmoPk_newFromFile("tests/pk.dat");
	if (pk->numPoints != 100)
		return false;

	if (islessgreater(pk->slopeBeyondKmax, -3.0))
		return false;

	if (islessgreater(pk->slopeBeforeKmin, 1.0))
		return false;

	if (islessgreater(pk->k[0], 0.000015))
		return false;

	if (islessgreater(pk->P[0], 1.49999e-05))
		return false;

	if (islessgreater(pk->k[99], 105.436996))
		return false;

	if (islessgreater(pk->P[99], 4.21748e-10))
		return false;

	cosmoPk_del(&pk);

	return true;
}

extern bool
cosmoPk_newFromArrays_test(void)
{
	cosmoPk_t pk;
	double    k[] = {1.0, 2.0, 3.0};
	double    P[] = {1.0, 5.0, 2.0};
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
#ifdef WITH_PROC_DIR
	char      buf[30];
	FILE      *pf;
	unsigned  sizeOld, sizeNew;
#endif

	printf("Testing %s... ", __func__);
#ifdef WITH_PROC_DIR
	snprintf(buf, 30, "/proc/%u/statm", (unsigned)getpid());
	pf = fopen(buf, "r");
	fscanf(pf, "%u", &sizeOld);
	rewind(pf);
#endif
	pk = cosmoPk_newFromFile("tests/pk.dat");
	cosmoPk_del(&pk);
#ifdef WITH_PROC_DIR
	fscanf(pf, "%u", &sizeNew);
	if (sizeOld != sizeNew)
		return false;
#endif

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
	double exponent = -1.0;
	bool hasSucceeded = true;

	printf("Testing %s... ", __func__);
	k = xmalloc(sizeof(double) * 100 * 2);
	P = k + 100;
	for (int i = 0; i < 100; i++) {
		k[i] = exp(log(0.1) + i * 0.1);
		P[i] = 1./k[i];
	}
	pk = cosmoPk_newFromArrays(100, k, P, -1.0, -1.0);
	cosmoPk_dumpToFile(pk, "test.dat", 100);

	sigmaSqrTheo = 1. / ((2. * 0. + 1.) * TWOPI * PI)
	               * (pow(k[99], 2. * 0. + 1.)
	                  - pow(k[10], 2. * 0. + 1.));
	sigmaSqr = cosmoPk_calcMomentFiltered(pk, UINT32_C(0),
	                                      &cosmoFunc_xPowerY,
	                                      &exponent,
	                                      k[10], k[99],
	                                      &error);
	if (isgreater(fabs(sigmaSqr/sigmaSqrTheo - 1.0),4e-5))
		hasSucceeded = false;
	sigmaSqrTheo = 1. / ((2. * 1. + 1.) * TWOPI * PI)
	               * (pow(k[99], 2. * 1. + 1.)
	                  - pow(k[10], 2. * 1. + 1.));
	sigmaSqr = cosmoPk_calcMomentFiltered(pk, UINT32_C(1),
	                                      &cosmoFunc_xPowerY,
	                                      &exponent,
	                                      k[10], k[99],
	                                      &error);
	if (isgreater(fabs(sigmaSqr/sigmaSqrTheo - 1.0),2e-4))
		hasSucceeded = false;

	xfree(k);
	cosmoPk_del(&pk);
	return hasSucceeded;
}

/*--- Implementations of local functions --------------------------------*/
