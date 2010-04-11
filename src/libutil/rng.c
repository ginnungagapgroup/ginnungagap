// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "rng.h"
#include "xmem.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef DEBUG
// SPRNG internal thing
#  define CHECK_POINTERS
#endif
#include <sprng.h>
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
		rng->streams[i] = init_sprng(rng->generatorType,
		                             rng->baseStreamId + i,
		                             rng->numStreamsTotal,
		                             rng->randomSeed,
		                             SPRNG_DEFAULT);
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

	for (int i = 0; i < (*rng)->numStreamsLocal; i++)
		free_rng((*rng)->streams[i]);
	xfree((*rng)->streams);
	xfree(*rng);
	*rng = NULL;
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
	double x, y, r2;

	do {
		x  = -1.0 + 2.0 * sprng(rng->streams[streamNumber]);
		y  = -1.0 + 2.0 * sprng(rng->streams[streamNumber]);
		r2 = x * x + y * y;
	} while (r2 > 1.0 || r2 < DBL_EPSILON);

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
