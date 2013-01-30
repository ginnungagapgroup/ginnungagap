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
#include <stdio.h>
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
#include "../../src/libutil/timer.h"
#include "../../src/libutil/diediedie.h"
#include "../../src/libutil/gadget.h"
#include "../../src/libutil/gadgetHeader.h"
#include "../../src/libutil/gadgetTOC.h"
#include "../../src/libg9p/g9pICMap.h"


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


/**
 * @brief  Helper function for generateICs_run().
 *
 * @param[in,out]  genics
 *                    The application to work with.
 * @param[in]      map
 *                    The mapping of tiles onto the files, include auxiliary
 *                    information about the number of cells in each file.
 * @param[in]      file
 *                    The file number to work on.
 *
 * @return  Returns nothing.
 */
static void
local_doFile(generateICs_t genics, const g9pICMap_t map, int file);


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
	if ((*genics)->hierarchy != NULL)
		g9pHierarchy_del(&(*genics)->hierarchy);
	if ((*genics)->datastore != NULL)
		g9pDataStore_del(&(*genics)->datastore);
	if ((*genics)->mask != NULL)
		g9pMask_del(&(*genics)->mask);
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
generateICs_setHierarchy(generateICs_t genics, g9pHierarchy_t hierarchy)
{
	assert(genics != NULL);

	if (genics->hierarchy != NULL) {
		fprintf(stderr, "ERROR: The hierarchy can only be set once.\n");
		diediedie(EXIT_FAILURE);
	}

	genics->hierarchy = hierarchy;
}

extern void
generateICs_setDataStore(generateICs_t genics, g9pDataStore_t datastore)
{
	assert(genics != NULL);

	if (genics->datastore != NULL) {
		fprintf(stderr, "ERROR: The datastore can only be set once.\n");
		diediedie(EXIT_FAILURE);
	}

	genics->datastore = datastore;
}

extern void
generateICs_setMask(generateICs_t genics, g9pMask_t mask)
{
	assert(genics != NULL);

	if (genics->mask != NULL) {
		fprintf(stderr, "ERROR: The mask can only be set once.\n");
		diediedie(EXIT_FAILURE);
	}

	genics->mask = mask;
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

extern g9pHierarchy_t
generateICs_getHierarchy(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->hierarchy;
}

extern g9pDataStore_t
generateICs_getDataStore(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->datastore;
}

extern g9pMask_t
generateICs_getMask(const generateICs_t genics)
{
	assert(genics != NULL);

	return genics->mask;
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

	g9pICMap_t map = g9pICMap_new(genics->numFiles, 0, NULL,
	                              g9pMask_getRef(genics->mask));

	if (genics->rank == 0)
		generateICs_printSummary(genics, stdout);

	for (uint32_t i = 0; i < genics->numFiles; i++) {
		printf(" * Working on file %i\n", i);
		double timing = timer_start();

		local_doFile(genics, map, i);

		timing = timer_stop(timing);
		printf("      File processed in in %.2fs\n", timing);
	}

	g9pICMap_del(&map);
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
	genics->hierarchy     = NULL;
	genics->datastore     = NULL;
	genics->mask          = NULL;
	genics->boxsizeInMpch = 0.0;
	genics->aInit         = 0.0;
	genics->doGas         = false;
	genics->doLongIDs     = false;
	genics->numFiles      = 0;
	genics->prefix        = NULL;
}

static void
local_doFile(generateICs_t genics, g9pICMap_t map, int file)
{
	uint32_t       firstTile      = g9pICMap_getFirstTileInFile(map, file);
	uint32_t       lastTile       = g9pICMap_getLastTileInFile(map, file);
	const uint64_t *numCells      = g9pICMap_getNumCellsPerLevelInFile(map,
	                                                                   file);
	const int8_t   numLevel       = g9pMask_getNumLevel(genics->mask);
	uint64_t       numPartsInFile = UINT64_C(0);
	for (int i = 0; i < numLevel; i++)
		numPartsInFile += numCells[i];

	fpv_t     *vel = xmalloc(sizeof(fpv_t) * numPartsInFile * 3);
	fpv_t     *pos = xmalloc(sizeof(fpv_t) * numPartsInFile * 3);
	const int sizeOfId
	    = genics->doLongIDs ? sizeof(uint32_t) : sizeof(uint64_t);
	void      *id = xmalloc(sizeOfId * numPartsInFile);

	for (uint32_t i = firstTile; i <= lastTile; i++) {
		uint64_t numCellsInTile[numLevel];
		(void)g9pMask_getNumCellsInTile(genics->mask, i, numCellsInTile);
		for (int8_t j = 0; j < numLevel; j++) {
			if (numCellsInTile[j] > UINT64_C(0)) {
#if 0
				patch = local_getPatchForTileOnLevel(i, j);

				reader = g9pDataStore_getReader(genics->datastore, j, vel_x);
				gridReader_readIntoPatchForVar(reader, patch, 0);
				reader = g9pDataStore_getReader(genics->datastore, j, vel_y);
				gridReader_readIntoPatchForVar(reader, patch, 1);
				reader = g9pDataStore_getReader(genics->datastore, j, vel_z);
				gridReader_readIntoPatchForVar(reader, patch, 2);
				local_copyToVel(vel + offset * 3, patch);

				local_setupIDs(((char *)id) + offset * sizeOfId, patch);
				local_setupPos(pos + offset * 3, patch);
#endif
			}
		}
	}

	xfree(id);
	xfree(pos);
	xfree(vel);
}
