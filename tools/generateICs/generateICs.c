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
local_writeGadgetFile(generateICs_t genics,
                      int           file,
                      fpv_t         *pos,
                      fpv_t         *vel,
                      void          *id,
                      uint64_t      numParticles);

static void
local_doTile(const gridPatch_t       patch,
             fpv_t                   *vel,
             fpv_t                   *pos,
             void                    *id,
             bool                    useLongIds,
             const gridPointUint32_t fullDims);


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

/*--- Exported function: Setter -----------------------------------------*/
extern void
generateICs_setMode(generateICs_t genics, generateICsMode_t mode)
{
	assert(genics != NULL);

	if (genics->mode != NULL)
		generateICsMode_del( &(genics->mode) );

	genics->mode = mode;
}

extern void
generateICs_setData(generateICs_t genics, generateICsData_t data)
{
	assert(genics != NULL);

	if (genics->data != NULL)
		generateICsData_del( &(genics->data) );

	genics->data = data;
}

extern void
generateICs_setIn(generateICs_t genics, generateICsIn_t in)
{
	assert(genics != NULL);

	if (genics->in != NULL)
		generateICsIn_del( &(genics->in) );

	genics->in = in;
}

extern void
generateICs_setOut(generateICs_t genics, generateICsOut_t out)
{
	assert(genics != NULL);

	if (genics->out != NULL)
		generateICsOut_del( &(genics->out) );

	genics->out = out;
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

/*--- Exported function: Getter -----------------------------------------*/
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

/*--- Exported function: Using ------------------------------------------*/
extern void
generateICs_printSummary(const generateICs_t genics, FILE *out)
{
	fprintf(out, "Using Gas     :  %s\n",
	        genics->mode->doGas ? "true" : "false");
	fprintf(out, "Using Long IDs:  %s\n",
	        genics->mode->useLongIDs ? "true" : "false");
}

extern void
generateICs_run(generateICs_t genics)
{
	assert(genics != NULL);

	g9pICMap_t map = g9pICMap_new( genics->out->numFiles, 0, NULL,
	                               g9pMask_getRef(genics->mask) );

	const uint32_t    tmp      = g9pMask_getDim1D(genics->mask);
	gridPointUint32_t fullDims = {tmp, tmp, tmp};
	generateICsOut_initBaseHeader(genics->out, genics->data, fullDims,
	                              genics->mode);

	if (genics->rank == 0)
		generateICs_printSummary(genics, stdout);

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

static void
local_doFile(generateICs_t genics, g9pICMap_t map, int file)
{
	uint32_t       firstTile      = g9pICMap_getFirstTileInFile(map, file);
	uint32_t       lastTile       = g9pICMap_getLastTileInFile(map, file);
	const uint64_t *numCells      = g9pICMap_getNumCellsPerLevelInFile(map,
	                                                                   file);
	const int8_t   numLevel       = g9pMask_getNumLevel(genics->mask);
	uint64_t       numPartsInFile = UINT64_C(4096);

	fpv_t             *vel = xmalloc(sizeof(fpv_t) * numPartsInFile * 3);
	fpv_t             *pos = xmalloc(sizeof(fpv_t) * numPartsInFile * 3);
	const int         sizeOfId
	    = genics->mode->useLongIDs ? sizeof(uint32_t) : sizeof(uint64_t);
	void              *id      = xmalloc(sizeOfId * numPartsInFile);

	uint64_t          offset   = 0;
	const uint32_t    tmp      = g9pMask_getDim1D(genics->mask);
	gridPointUint32_t fullDims = {tmp, tmp, tmp};

	dataVar_t         varVelx, varVely, varVelz;
	varVelx = dataVar_new("velx", DATAVARTYPE_FPV, 1);
	varVely = dataVar_new("vely", DATAVARTYPE_FPV, 1);
	varVelz = dataVar_new("velz", DATAVARTYPE_FPV, 1);

	printf("numPartsInFile = %lu\n", numPartsInFile);
	printf("fullDims = (%u, %u, %u)\n", fullDims[0], fullDims[1], fullDims[2]);

	for (uint32_t i = firstTile; i <= lastTile; i++) {
		uint64_t numCellsInTile[numLevel];
		(void)g9pMask_getNumCellsInTile(genics->mask, i, numCellsInTile);
		for (int8_t j = 0; j < numLevel; j++) {
			assert(j == 0); // Only for one level at the moment
			if ( numCellsInTile[j] > UINT64_C(0) ) {
				gridPatch_t patch = g9pMask_getEmptyPatchForTile(genics->mask,
				                                                 i);
				(void)gridPatch_attachVar(patch, varVelx);
				(void)gridPatch_attachVar(patch, varVely);
				(void)gridPatch_attachVar(patch, varVelz);

				printf("Working on tile %u, level %i (%i cells)\n",
				       i, (int)j, (int)numCellsInTile[j]);

				//reader = g9pDataStore_getReader(genics->datastore, j, vel_x);
				gridReader_readIntoPatchForVar(genics->in->velx, patch, 0);
				//reader = g9pDataStore_getReader(genics->datastore, j, vel_y);
				gridReader_readIntoPatchForVar(genics->in->vely, patch, 1);
				//reader = g9pDataStore_getReader(genics->datastore, j, vel_z);
				gridReader_readIntoPatchForVar(genics->in->velz, patch, 2);

				local_doTile(patch, vel + offset * 3, pos + offset * 3,
				             ( (char *)id + offset * sizeOfId ),
				             genics->mode->useLongIDs, fullDims);
				offset += numCellsInTile[j];

				gridPatch_del(&patch);
			}
		}
	}

	dataVar_del(&varVelx);
	dataVar_del(&varVely);
	dataVar_del(&varVelz);

	local_writeGadgetFile(genics, file, pos, vel, id, numPartsInFile);

	xfree(id);
	xfree(pos);
	xfree(vel);
} // local_doFile

static void
local_writeGadgetFile(generateICs_t genics,
                      int           file,
                      fpv_t         *pos,
                      fpv_t         *vel,
                      void          *id,
                      uint64_t      numParticles)
{
	uint32_t       npLocal[6] = {0, numParticles, 0, 0, 0, 0};
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
		stai = stai_new( pos, 3 * sizeof(float), 3 * sizeof(float) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_POS_,
		                               0, numParticles, stai);
		stai_del(&stai);
		stai = stai_new( vel, 3 * sizeof(float), 3 * sizeof(float) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_VEL_,
		                               0, numParticles, stai);
		stai_del(&stai);
		if (genics->mode->useLongIDs)
			stai = stai_new( id, sizeof(uint64_t), sizeof(uint64_t) );
		else
			stai = stai_new( id, sizeof(uint32_t), sizeof(uint32_t) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_ID__,
		                               0, numParticles, stai);
		stai_del(&stai);
	}
	gadget_close(genics->out->gadget);
} // local_writeGadgetFile

static void
local_doTile(const gridPatch_t       patch,
             fpv_t                   *vel,
             fpv_t                   *pos,
             void                    *id,
             bool                    useLongIds,
             const gridPointUint32_t fullDims)
{
	gridPointUint32_t dims, idxLo;

	fpv_t             *velxP = gridPatch_getVarDataHandle(patch, 0);
	fpv_t             *velyP = gridPatch_getVarDataHandle(patch, 1);
	fpv_t             *velzP = gridPatch_getVarDataHandle(patch, 2);

	double            dx     = 1.0;

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);
	printf("Patch idxLo: (%u,%u,%u)\n", idxLo[0], idxLo[1], idxLo[2]);
	printf("Patch dims:  (%u,%u,%u)\n", dims[0], dims[1], dims[2]);

	gridPointUint32_t p;
	uint64_t          i = 0;
	for (p[2] = idxLo[2]; p[2] < idxLo[2] + dims[2]; p[2]++) {
		for (p[1] = idxLo[1]; p[1] < idxLo[1] + dims[1]; p[1]++) {
			for (p[0] = idxLo[0]; p[0] < idxLo[0] + dims[0]; p[0]++) {
				vel[i * 3]     = velxP[i];
				vel[i * 3 + 1] = velyP[i];
				vel[i * 3 + 2] = velzP[i];

				pos[i * 3]     = (fpv_t)( (p[0] + .5) * dx );
				pos[i * 3 + 1] = (fpv_t)( (p[1] + .5) * dx );
				pos[i * 3 + 2] = (fpv_t)( (p[2] + .5) * dx );

				if (!useLongIds) {
					( (uint32_t *)id )[i] = lIdx_fromCoord3d(p, fullDims);
				} else {
					( (uint64_t *)id )[i] = lIdx_fromCoord3d(p, fullDims);
				}
				i++;
			}
		}
	}
} // local_doTile
