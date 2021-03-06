// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pInit.c
 * @ingroup  ginnungagapInit
 * @brief  Provides the implementation of the initialistion routine.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pInit.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libcosmo/cosmoPk.h"
#include "../libcosmo/cosmoModel.h"
#include "../libcosmo/cosmoFunc.h"
#include "../libcosmo/cosmo.h"
#include "../libutil/utilMath.h"
#include "../libutil/diediedie.h"


/*--- Local structures --------------------------------------------------*/
typedef struct localFreqs_struct {
	double minBox;
	double maxBox;
	double minPk;
	double maxPk;
} localFreqs_struct_t;

typedef struct localFreqs_struct *localFreqs_t;

/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static inline void
local_calcAndCheckFreqs(localFreqs_t k,
                        double       boxsizeInMpch,
                        uint32_t     dim1D,
                        cosmoPk_t    pk,
                        int          rank);

static inline void
local_renormalizePk(localFreqs_t   k,
                    cosmoPk_t      pk,
                    cosmoModel_t   model,
                    g9pNorm_mode_t mode,
                    int            rank);

static inline void
local_shiftPkToAInit(cosmoModel_t model,
                     cosmoPk_t    pk,
                     double       aInit,
                     int          rank);

static inline void
local_calcSigmaBox(localFreqs_t k,
                   cosmoPk_t    pk,
                   int          rank,
                   double       maxSigmaExpected);


/**
 * @brief  Calculates the largest expected sigma.
 *
 * This assume a Gaussian distribution with zero mean and unit variance,
 * i.e.
 * @f[
 *   f(x) = \frac{1}{\sqrt{2 \pi}} \exp{-x^2/2.}
 * @f]
 * and then evaluates
 * @f[
 *   f(x) N^3 \equiv 1
 * @f]
 * to find the largest @c x that would be expected when the distribution is
 * sampled with @f$ N^3 @f$ points.
 *
 * @return  Returns the largest expected sigma.
 */
static inline double
local_calcMaxSigmaExpected(uint32_t n);


/*--- Implementations of exported functios ------------------------------*/
extern void
g9pInit_init(double         boxsizeInMpch,
             uint32_t       dim1D,
             double         zInit,
             cosmoPk_t      pk,
             cosmoModel_t   model,
             g9pNorm_mode_t normalisationMode,
             const char     *namePkInput,
             const char     *namePkInputZ0,
             const char     *namePkInputZinit)
{
	localFreqs_struct_t k;
	int                 rank = 0;

	assert(isgreater(boxsizeInMpch, 0.0));
	assert(dim1D > 0);
	assert(isgreaterequal(zInit, 0.0));
	assert(pk != NULL);
	assert(model != NULL);

#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	local_calcAndCheckFreqs(&k, boxsizeInMpch, dim1D, pk, rank);
	cosmoPk_dumpToFile(pk, namePkInput, 10);
	local_renormalizePk(&k, pk, model, normalisationMode, rank);
	cosmoPk_dumpToFile(pk, namePkInputZ0, 10);
	local_shiftPkToAInit(model, pk, cosmo_z2a(zInit), rank);
	cosmoPk_dumpToFile(pk, namePkInputZinit, 10);
	local_calcSigmaBox(&k, pk, rank, local_calcMaxSigmaExpected(dim1D));
}

/*--- Implementations of local functions --------------------------------*/
static inline void
local_calcAndCheckFreqs(localFreqs_t k,
                        double       boxsizeInMpch,
                        uint32_t     dim1D,
                        cosmoPk_t    pk,
                        int          rank)
{
	bool kMinOkay, kMaxOkay;

	k->minBox = 2. * M_PI / boxsizeInMpch;
	k->maxBox = dim1D * M_PI / boxsizeInMpch;
	k->minPk  = cosmoPk_getKminSecure(pk);
	k->maxPk  = cosmoPk_getKmaxSecure(pk);

	kMinOkay  = isgreater(k->minBox, 1.05 * k->minPk) ?
	            true : false;
	kMaxOkay  = isless(sqrt(3) * k->maxBox, 0.95 * k->maxPk) ?
	            true : false;

	if (rank == 0) {
		printf("\n  Window  :  kmin = %e h/Mpc  kmax = %e h/Mpc\n",
		       k->minPk, k->maxPk);
		printf("  Window 2:  kmin = %e h/Mpc  kmax = %e h/Mpc\n",
		       1.05 * k->minPk, 0.95 * k->maxPk);
		printf("  Box     :  kmin = %e h/Mpc  kmax = %e h/Mpc\n",
		       k->minBox, k->maxBox);
		if ((rank == 0) && !kMinOkay)
			fprintf(stderr, "kmin = %f of P(k) is too large!\n", k->minPk);
		if ((rank == 0) && !kMaxOkay)
			fprintf(stderr, "kmax = %f of P(k) is too small!\n", k->maxPk);
	}

	if (!kMinOkay || !kMaxOkay)
		diediedie(EXIT_FAILURE);
}

static inline void
local_renormalizePk(localFreqs_t   k,
                    cosmoPk_t      pk,
                    cosmoModel_t   model,
                    g9pNorm_mode_t mode,
                    int            rank)
{
	double s8Orig, s8Trgt, s8Pk, s8Pk2, s8Box, error;

	s8Orig = cosmoPk_calcSigma8(pk, k->minPk, k->maxPk, &error);
	s8Trgt = cosmoModel_getSigma8(model);

	if (mode == G9PNORM_MODE_SIGMA8)
		(void)cosmoPk_forceSigma8(pk, s8Trgt, k->minPk, k->maxPk, &error);
	else if (mode == G9PNORM_MODE_SIGMA8BOX)
		(void)cosmoPk_forceSigma8(pk, s8Trgt, k->minBox, k->maxBox, &error);

	s8Pk  = cosmoPk_calcSigma8(pk, k->minPk, k->maxPk, &error);
	s8Pk2 = cosmoPk_calcSigma8(pk, 1.05 * k->minPk, .95 * k->maxPk, &error);
	s8Box = cosmoPk_calcSigma8(pk, k->minBox, k->maxBox, &error);

	if (rank == 0) {
		printf("\n  sigma8 of original P(k):  %.7f\n", s8Orig);
		printf("  sigma8 of model        :  %.7f\n", s8Trgt);
		printf("  sigma8 in window       :  %.7f\n", s8Pk);
		printf("  sigma8 in window2      :  %.7f\n", s8Pk2);
		printf("  rel. difference w/w2-1 :  %e\n", s8Pk / s8Pk2 - 1.);
		printf("  sigma8 in box          :  %.7f\n", s8Box);
		printf("  normalisation mode     :  %s\n",
		       g9pNorm_getNameFromMode(mode));
	}
}

static inline void
local_shiftPkToAInit(cosmoModel_t model,
                     cosmoPk_t    pk,
                     double       aInit,
                     int          rank)
{
	double growth0, growth1, scale, error;

	growth0 = cosmoModel_calcGrowth(model, 1.0, &error);
	growth1 = cosmoModel_calcGrowth(model, aInit, &error);

	scale   = growth1 / growth0;
	scale  *= scale;
	cosmoPk_scale(pk, scale);

	if (rank == 0) {
		printf("\n  D(z =%6.2f):  %f\n", 0.0, growth0);
		printf("  D(z =%6.2f):  %f\n", cosmo_a2z(aInit), growth1);
		printf("  scale * P(k,%6.2f) = P(k,%6.2f):  scale = %f\n",
		       0.0, cosmo_a2z(aInit), scale);
	}
}

static inline void
local_calcSigmaBox(localFreqs_t k,
                   cosmoPk_t    pk,
                   int          rank,
                   double       maxSigmaExpected)
{
	double        sigmaBox, error;
	static double one = 1.0;

	sigmaBox = cosmoPk_calcMomentFiltered(pk, 0, &cosmoFunc_const, &one,
	                                      k->minBox, k->maxBox, &error);
	sigmaBox = sqrt(sigmaBox);

	if (rank == 0) {
		printf("\n  maxSigmaExpected:  %f", maxSigmaExpected);
		printf("\n  sigmaBox        :  %f", sigmaBox);
		if (isless(maxSigmaExpected * sigmaBox, 1.)) {
			printf("\n    maxSigmaExpected * sigmaBox  = %f < 1, good.\n",
			       maxSigmaExpected * sigmaBox);
		} else {
			fprintf(stderr,
			        "\nERROR: maxSigmaExpected * sigmaBox = %f >= 1.\n",
			        maxSigmaExpected * sigmaBox);
			diediedie(EXIT_FAILURE);
		}
	}
}

static inline double
local_calcMaxSigmaExpected(uint32_t n)
{
	return sqrt(-2. * log(sqrt(2. * M_PI) / POW_NDIM((double)n)));
}
