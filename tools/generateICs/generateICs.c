// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICs.c
 * @ingroup  toolsGICSApp
 * @brief  Provides the implementation of the @c generateICs tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICs.h"
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_OPENMP
#  include <omp.h>
#endif
#include "../../src/libcosmo/cosmoModel.h"
#include "../../src/libcosmo/cosmo.h"
#include "../../src/libutil/utilMath.h"
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/gadget.h"
#include "../../src/libutil/gadgetHeader.h"
#include "../../src/libutil/gadgetTOC.h"


/*--- Implemention of main structure ------------------------------------*/
#include "generateICs_adt.h"


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Allocates memory for the generateICs structure.
 *
 * @return  Returns a new handle for a generateICs object.
 */
inline static generateICs_t
local_alloc(void);


/**
 * @brief  Initialises a generateICs object with safe values.
 *
 * @param[in,out]  genics
 *                    The structure that should be initialized.  Passing
 *                    @c NULL is undefined.  Existing information will be
 *                    cleaned before resetting to safe values.
 *
 * @return  Returns nothing.
 */
inline static void
local_init(generateICs_t genics);


/*--- Exported functions: Creating and deleting -------------------------*/
extern generateICs_t
generateICs_new(void)
{
	generateICs_t genics;

	genics = local_alloc();
	local_init(genics);

	return genics;
}

extern void
generateICs_del(generateICs_t *genics)
{
	assert(genics != NULL);
	assert(*genics != NULL);

	if ((*genics)->model != NULL)
		cosmoModel_del(&(*genics)->model);
	if ((*genics)->prefix != NULL)
		xfree((*genics)->prefix);

	xfree(*genics);

	*genics = NULL;
}

/*--- Exported function: Setter -----------------------------------------*/
extern void
generateICs_setCosmoModel(generateICs_t genics, cosmoModel_t model)
{
	assert(genics != NULL);

	if (genics->model != NULL)
		cosmoModel_del(&model);

	genics->model = model;
}

extern void
generateICs_setBoxsizeInMpch(generateICs_t genics, double boxsizeInMpch)
{
	assert(genics != NULL);
	assert(isgreater(boxsizeInMpch, 0.0));

	genics->boxsizeInMpch = boxsizeInMpch;
}

extern void
generateICs_setAInit(generateICs_t genics, double aInit)
{
	assert(genics != NULL);
	assert(isgreater(aInit, 0.0));

	genics->aInit = aInit;
}

extern void
generateICs_setDoGas(generateICs_t genics, bool doGas)
{
	assert(genics != NULL);

	genics->doGas = doGas;
}

extern void
generateICs_setDoLongIDs(generateICs_t genics, bool doLongIDs)
{
	assert(genics != NULL);

	genics->doLongIDs = doLongIDs;
}

extern void
generateICs_setNumFiles(generateICs_t genics, uint32_t numFiles)
{
	assert(genics != NULL);

	genics->numFiles = numFiles;
}

extern void
generateICs_setPrefix(generateICs_t genics, char *prefix)
{
	assert(genics != NULL);

	if (genics->prefix != NULL)
		xfree(genics->prefix);

	genics->prefix = prefix;
}

/*--- Exported function: Getter -----------------------------------------*/
extern cosmoModel_t
generateICs_getCosmoModel(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->model;
}

extern double
generateICs_getBoxsizeInMpch(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->boxsizeInMpch;
}

extern double
generateICs_getAinit(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->aInit;
}

extern bool
generateICs_getDoGas(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->doGas;
}

extern bool
generateICs_getLongIDs(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->doLongIDs;
}

extern uint32_t
generateICs_getNumFiles(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->numFiles;
}

extern const char *
generateICs_getPrefix(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->prefix;
}

/*--- Exported function: Using ------------------------------------------*/
extern void
generateICs_printSummary(const generateICs_t genics, FILE *out)
{
	fprintf(out, "Using Gas     :  %s\n",
	        genics->doGas ? "true" : "false");
	fprintf(out, "Using Long IDs:  %s\n",
	        genics->doLongIDs ? "true" : "false");
}

extern void
generateICs_run(generateICs_t genics)
{
	assert(genics != NULL);

	if (genics->rank == 0)
		generateICs_printSummary(genics, stdout);

	for (uint32_t i = 0; i < genics->numFiles; i++) {
		if (genics->rank == 0)
			fprintf(stdout, "  * Working on file %" PRIu32 "\n");
		// DO WORK
		if (genics->rank == 0)
			fprintf(stdout, "    done\n");
	}
}

/*--- Implementations of local functions --------------------------------*/
inline static generateICs_t
local_alloc(void)
{
	return xmalloc(sizeof(struct generateICs_struct));
}

inline static void
local_init(generateICs_t genics)
{
	assert(genics != NULL);

#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &(genics->size));
	MPI_Comm_rank(MPI_COMM_WORLD, &(genics->rank));
#else
	genics->size = 1;
	genics->rank = 0;
#endif

#ifdef WITH_OPENMP
	genics->numThreads = omp_get_max_threads();
#else
	genics->numThreads = 1;
#endif

	genics->model         = NULL;
	genics->boxsizeInMpch = 0.0;
	genics->aInit         = 0.0;
	genics->doGas         = false;
	genics->doLongIDs     = false;
	genics->numFiles      = 0;
	genics->prefix        = NULL;
}
