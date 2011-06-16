// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/rng.c
 * @ingroup libutilFilesMisc
 * @brief  This file provides the implementation of the RNG.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "rng.h"
#include "xmem.h"
#include "diediedie.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_SPRNG
#  ifdef DEBUG
#    define CHECK_POINTERS
#  endif
#  include <sprng.h>
#endif
#include <math.h>
#include <float.h>
#include <assert.h>


/*--- Implemention of main structure ------------------------------------*/
#include "rng_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define CONFIG_GENERATOR_NAME    "generator"
#define CONFIG_TOTALSTREAMS_NAME "numStreamsTotal"
#define CONFIG_RANDOMSEED_NAME   "randomSeed"


/*--- Prototypes of local functions -------------------------------------*/
static int
local_getGeneratorType(parse_ini_t ini, const char *sectionName);

static int
local_getNumStreamsTotal(parse_ini_t ini, const char *sectionName);

static int
local_getRandomSeed(parse_ini_t ini, const char *sectionName);

static int
local_getNumStreamsLocal(int numStreamsTotal);

static int
local_getBaseStreamId(int numStreamsTotal);


/*--- Implementations of exported functios ------------------------------*/
extern rng_t
rng_new(int generatorType, int numStreamsTotal, int randomSeed)
{
	rng_t rng;

	rng                  = xmalloc(sizeof(struct rng_struct));
	rng->generatorType   = generatorType;
	rng->numStreamsTotal = numStreamsTotal;
	rng->randomSeed      = randomSeed;
	rng->numStreamsLocal = local_getNumStreamsLocal(rng->numStreamsTotal);
	rng->baseStreamId    = local_getBaseStreamId(rng->numStreamsLocal);

	assert(rng->baseStreamId + rng->numStreamsLocal <= rng->numStreamsTotal);

	rng->streams = xmalloc(sizeof(int *) * rng->numStreamsLocal);
	for (int i = 0; i < rng->numStreamsLocal; i++) {
#ifdef WITH_SPRNG
		rng->streams[i] = init_sprng(rng->generatorType,
		                             rng->baseStreamId + i,
		                             rng->numStreamsTotal,
		                             rng->randomSeed,
		                             SPRNG_DEFAULT);
#else
		rng->streams[i] = NULL;
#endif
	}

	return rng;
}

extern rng_t
rng_newFromIni(parse_ini_t ini, const char *sectionName)
{
	int generatorType   = local_getGeneratorType(ini, sectionName);
	int numStreamsTotal = local_getNumStreamsTotal(ini, sectionName);
	int randomSeed      = local_getRandomSeed(ini, sectionName);

	return rng_new(generatorType, numStreamsTotal, randomSeed);
}

extern void
rng_del(rng_t *rng)
{
	assert(rng != NULL);
	assert(*rng != NULL);

#ifdef WITH_SPRNG
	for (int i = 0; i < (*rng)->numStreamsLocal; i++)
		free_rng((*rng)->streams[i]);
#endif
	xfree((*rng)->streams);
	xfree(*rng);
	*rng = NULL;
}

extern void
rng_reset(rng_t rng)
{
#ifdef WITH_SPRNG
	for (int i = 0; i < rng->numStreamsLocal; i++) {
		free_rng(rng->streams[i]);
		rng->streams[i] = init_sprng(rng->generatorType,
		                             rng->baseStreamId + i,
		                             rng->numStreamsTotal,
		                             rng->randomSeed,
		                             SPRNG_DEFAULT);
	}
#endif
}

extern int
rng_getNumStreamsLocal(const rng_t rng)
{
	assert(rng != NULL);
	return rng->numStreamsLocal;
}

extern double
rng_getGauss(const rng_t  rng,
             const int    streamNumber,
             const double mean,
             const double sigma)
{
#ifdef WITH_SPRNG
	double x, y, r2;

	do {
		x  = -1.0 + 2.0 * sprng(rng->streams[streamNumber]);
		y  = -1.0 + 2.0 * sprng(rng->streams[streamNumber]);
		r2 = x * x + y * y;
	} while ((r2 > 1.0) || (r2 == 0.0));
#else
	diediedie(EXIT_FAILURE);
#endif

	return sigma * y * sqrt(-2.0 * log(r2) / r2) + mean;
}

extern double
rng_getGaussUnit(const rng_t rng, const int streamNumber)
{
	return rng_getGauss(rng, streamNumber, 0.0, 1.0);
}

/*--- Implementations of local functions --------------------------------*/
static int
local_getGeneratorType(parse_ini_t ini, const char *sectionName)
{
	int32_t tmp;
	getFromIni(&tmp, parse_ini_get_int32,
	           ini, CONFIG_GENERATOR_NAME, sectionName);
	if ((tmp < INT32_C(0)) || (tmp > INT32_C(5))) {
		fprintf(stderr, "FATAL:  Generator type %i unknown!.\n",
		        (int)tmp);
		exit(EXIT_FAILURE);
	}
	return (int)tmp;
}

static int
local_getNumStreamsTotal(parse_ini_t ini, const char *sectionName)
{
	int32_t tmp;
	getFromIni(&tmp, parse_ini_get_int32,
	           ini, CONFIG_TOTALSTREAMS_NAME, sectionName);
	if (tmp < INT32_C(1)) {
		fprintf(stderr, "FATAL:  Cannot use less than 1 stream!\n");
		exit(EXIT_FAILURE);
	}
	return (int)tmp;
}

static int
local_getRandomSeed(parse_ini_t ini, const char *sectionName)
{
	int32_t tmp;
	getFromIni(&tmp, parse_ini_get_int32,
	           ini, CONFIG_RANDOMSEED_NAME, sectionName);
	return (int)tmp;
}

static int
local_getNumStreamsLocal(int numStreamsTotal)
{
	int size = 1;
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif
	if (numStreamsTotal < size) {
		fprintf(stderr,
		        "FATAL:  Cannot use less streams than MPI tasks!\n");
		exit(EXIT_FAILURE);
	}
	if (numStreamsTotal % size != 0) {
		fprintf(stderr,
		        "FATAL:  numStreamsTotal must be an integer multiple "
		        "of the number of tasks (size = %i)!\n", size);
		exit(EXIT_FAILURE);
	}
	return numStreamsTotal / size;
}

static int
local_getBaseStreamId(int numStreamsLocal)
{
	int rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	return rank * numStreamsLocal;
}
