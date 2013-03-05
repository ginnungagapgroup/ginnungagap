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
#include "generateICsCore.h"
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
#include "../../src/libutil/lIdx.h"
#include "../../src/libutil/gadget.h"
#include "../../src/libutil/gadgetHeader.h"
#include "../../src/libutil/gadgetTOC.h"
#include "../../src/libg9p/g9pICMap.h"
#include "../../src/libgrid/gridPatch.h"


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

static void
local_setupCore(generateICsCore_t   core,
                const generateICs_t genics,
                uint32_t            tile);


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

static void
local_writeGadgetFile(generateICs_t           genics,
                      int                     file,
                      generateICsCore_const_t core);


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

	if ( (*genics)->mode != NULL )
		generateICsMode_del( &( (*genics)->mode ) );
	if ( (*genics)->data != NULL )
		generateICsData_del( &( (*genics)->data ) );
	if ( (*genics)->in != NULL )
		generateICsIn_del( &( (*genics)->in ) );
	if ( (*genics)->out != NULL )
		generateICsOut_del( &( (*genics)->out ) );
	if ( (*genics)->hierarchy != NULL )
		g9pHierarchy_del(&(*genics)->hierarchy);
	if ( (*genics)->datastore != NULL )
		g9pDataStore_del(&(*genics)->datastore);
	if ( (*genics)->mask != NULL )
		g9pMask_del(&(*genics)->mask);

	xfree(*genics);

	*genics = NULL;
} // generateICs_del

/*--- Include getter/setter function ------------------------------------*/
#include "generateICs_impl.c"


/*--- Exported function: Using ------------------------------------------*/
extern void
generateICs_printSummary(const generateICs_t genics, FILE *out)
{
	fprintf(out, "Using Gas      :  %s\n",
	        genics->mode->doGas ? "true" : "false");
	fprintf(out, "Using Long IDs :  %s\n",
	        genics->mode->useLongIDs ? "true" : "false");
	fprintf(out, "Boxsize [Mpc/h]: %lf\n", genics->data->boxsizeInMpch);
	fprintf(out, "A_init         : %lf\n", genics->data->aInit);
	fprintf(out, "vFact          : %lf\n", genics->data->vFact);
	fprintf(out, "aDot           : %lf\n", genics->data->adot);
	fprintf(out, "growthVel      : %lf\n", genics->data->growthVel);
	fprintf(out, "posFactor      : %lf\n", genics->data->posFactor);
	fprintf(out, "velFactor      : %lf\n", genics->data->velFactor);
}

extern void
generateICs_run(generateICs_t genics)
{
	assert(genics != NULL);

	g9pICMap_t map = g9pICMap_new( genics->out->numFiles, 0, NULL,
	                               g9pMask_getRef(genics->mask) );

	if (genics->rank == 0)
		generateICs_printSummary(genics, stdout);

	const uint32_t    tmp      = g9pMask_getDim1D(genics->mask);
	gridPointUint32_t fullDims = {tmp, tmp, tmp};
	generateICsOut_initBaseHeader(genics->out, genics->data, fullDims,
	                              genics->mode);

	for (uint32_t i = 0; i < genics->out->numFiles; i++) {
		printf(" * Working on file %i\n", i);
		double timing = timer_start();

		local_doFile(genics, map, i);

		timing = timer_stop(timing);
		printf("      File processed in in %.2fs\n", timing);
	}

	g9pICMap_del(&map);
} // generateICs_run

/*--- Implementations of local functions --------------------------------*/
inline static generateICs_t
local_alloc(void)
{
	return xmalloc( sizeof(struct generateICs_struct) );
}

inline static void
local_init(generateICs_t genics)
{
	assert(genics != NULL);

#ifdef WITH_MPI
	MPI_Comm_size( MPI_COMM_WORLD, &(genics->size) );
	MPI_Comm_rank( MPI_COMM_WORLD, &(genics->rank) );
#else
	genics->size = 1;
	genics->rank = 0;
#endif

#ifdef WITH_OPENMP
	genics->numThreads = omp_get_max_threads();
#else
	genics->numThreads = 1;
#endif

	genics->mode      = NULL;
	genics->data      = NULL;
	genics->in        = NULL;
	genics->out       = NULL;
	genics->hierarchy = NULL;
	genics->datastore = NULL;
	genics->mask      = NULL;
} // local_init

static uint64_t
local_computeNumParts(const generateICs_t genics, uint32_t tile)
{
	uint64_t np;

	np = g9pMask_getNumCellsInTileForLevel( genics->mask, tile,
	                                        g9pMask_getMaxLevel(genics->mask) );

	return np;
}

static void
local_setupCore(generateICsCore_t   core,
                const generateICs_t genics,
                uint32_t            tile)
{
	core->numParticles = local_computeNumParts(genics, tile);
	core->pos          = xmalloc(sizeof(fpv_t) * (core->numParticles) * 3);
	core->vel          = xmalloc(sizeof(fpv_t) * (core->numParticles) * 3);
	if (genics->mode->useLongIDs)
		core->id = xmalloc(sizeof(uint64_t) * core->numParticles);
	else
		core->id = xmalloc(sizeof(uint32_t) * core->numParticles);

	core->fullDims[0] = g9pMask_getDim1D(genics->mask);
	core->fullDims[1] = core->fullDims[0];
	core->fullDims[2] = core->fullDims[0];

	printf("numPartsInFile = %lu\n", core->numParticles);
	printf("fullDims = (%u, %u, %u)\n", core->fullDims[0],
	       core->fullDims[1], core->fullDims[2]);
} // local_setupCore

static void
local_doFile(generateICs_t genics, g9pICMap_t map, int file)
{
	uint32_t          firstTile = g9pICMap_getFirstTileInFile(map, file);
	uint32_t          lastTile  = g9pICMap_getLastTileInFile(map, file);

	generateICsCore_s core      = GENICSCORE_INIT_STRUCT(genics->data,
	                                                     genics->mode);

	for (uint32_t i = firstTile; i <= lastTile; i++) {
		assert(i == firstTile);
		local_setupCore(&core, genics, i);
		core.patch = g9pMask_getEmptyPatchForTile(genics->mask, i);
		(void)gridPatch_attachVar(core.patch, genics->in->varVelx);
		(void)gridPatch_attachVar(core.patch, genics->in->varVely);
		(void)gridPatch_attachVar(core.patch, genics->in->varVelz);

		gridReader_readIntoPatchForVar(genics->in->velx, core.patch, 0);
		gridReader_readIntoPatchForVar(genics->in->vely, core.patch, 1);
		gridReader_readIntoPatchForVar(genics->in->velz, core.patch, 2);

		generateICsCore_toParticles(&core);

		gridPatch_del( &(core.patch) );
	}

	local_writeGadgetFile(genics, file, &core);

	xfree(core.id);
	xfree(core.pos);
	xfree(core.vel);
} // local_doFile

static void
local_writeGadgetFile(generateICs_t           genics,
                      int                     file,
                      generateICsCore_const_t core)
{
	uint32_t       npLocal[6] = {0, core->numParticles, 0, 0, 0, 0};
	double         massArr[6] = {0., 0., 0., 0., 0., 0.};
	gadgetHeader_t myHeader;

	myHeader = gadgetHeader_clone(genics->out->baseHeader);
	gadgetHeader_setNp(myHeader, npLocal);
	gadgetTOC_calcSizes(genics->out->toc, npLocal, massArr, false,
	                    genics->mode->useLongIDs);
	gadgetTOC_calcOffset(genics->out->toc);
	gadget_setHeaderOfFile(genics->out->gadget, file, myHeader);
	gadget_setTOCOfFile( genics->out->gadget, file,
	                     gadgetTOC_clone(genics->out->toc) );
	gadget_open(genics->out->gadget, GADGET_MODE_WRITE_CREATE, file);
	gadget_writeHeaderToCurrentFile(genics->out->gadget);
	{
		stai_t stai;
		stai = stai_new( core->pos, 3 * sizeof(float), 3 * sizeof(float) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_POS_,
		                               0, core->numParticles, stai);
		stai_del(&stai);
		stai = stai_new( core->vel, 3 * sizeof(float), 3 * sizeof(float) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_VEL_,
		                               0, core->numParticles, stai);
		stai_del(&stai);
		if (genics->mode->useLongIDs)
			stai = stai_new( core->id, sizeof(uint64_t), sizeof(uint64_t) );
		else
			stai = stai_new( core->id, sizeof(uint32_t), sizeof(uint32_t) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_ID__,
		                               0, core->numParticles, stai);
		stai_del(&stai);
	}
	gadget_close(genics->out->gadget);
} // local_writeGadgetFile
