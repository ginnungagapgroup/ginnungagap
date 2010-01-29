// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "rng.h"
#include "../libutil/xmem.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef DEBUG
#  define CHECK_POINTERS
#endif
#include <sprng.h>
#include <assert.h>


/*--- Implemention of main structure ------------------------------------*/
struct rng_struct {
	int **streams;
	int generatorType;
	int baseStreamId;
	int numStreamsTotal;
	int numStreamsLocal;
	int randomSeed;
};


/*--- Local defines -----------------------------------------------------*/
#define CONFIG_SECTION_NAME      "rng"
#define CONFIG_GENERATOR_NAME    "generator"
#define CONFIG_TOTALSTREAMS_NAME "numStreamsTotal"
#define CONFIG_RANDOMSEED_NAME   "randomSeed"


/*--- Prototypes of local functions -------------------------------------*/
static int
local_getGeneratorType(parse_ini_t ini);

static int
local_getNumStreamsTotal(parse_ini_t ini);

static int
local_getRandomSeed(parse_ini_t ini);

static int
local_getNumStreamsLocal(int numStreamsTotal);

static int
local_getBaseStreamId(int numStreamsTotal);


/*--- Implementations of exported functios ------------------------------*/
extern rng_t
rng_new(parse_ini_t ini)
{
	rng_t rng;

	rng                  = xmalloc(sizeof(struct rng_struct));
	rng->generatorType   = local_getGeneratorType(ini);
	rng->numStreamsTotal = local_getNumStreamsTotal(ini);
	rng->randomSeed      = local_getRandomSeed(ini);
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

/*--- Implementations of local functions --------------------------------*/
static int
local_getGeneratorType(parse_ini_t ini)
{
	int32_t tmp;
	getFromIni(&tmp, parse_ini_get_int32,
	           ini, CONFIG_GENERATOR_NAME, CONFIG_SECTION_NAME);
	if ((tmp < INT32_C(0)) || (tmp > INT32_C(5))) {
		fprintf(stderr, "FATAL:  Generator type %i unknown!.\n",
		        (int)tmp);
		exit(EXIT_FAILURE);
	}
	return (int)tmp;
}

static int
local_getNumStreamsTotal(parse_ini_t ini)
{
	int32_t tmp;
	getFromIni(&tmp, parse_ini_get_int32,
	           ini, CONFIG_TOTALSTREAMS_NAME, CONFIG_SECTION_NAME);
	if (tmp < INT32_C(1)) {
		fprintf(stderr, "FATAL:  Cannot use less than 1 stream!\n");
		exit(EXIT_FAILURE);
	}
	return (int)tmp;
}

static int
local_getRandomSeed(parse_ini_t ini)
{
	int32_t tmp;
	getFromIni(&tmp, parse_ini_get_int32,
	           ini, CONFIG_RANDOMSEED_NAME, CONFIG_SECTION_NAME);
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
		        "of the number of MPI tasks!\n");
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
