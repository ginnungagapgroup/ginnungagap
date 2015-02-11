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
#include "../../src/libpart/partBunch.h"
#include "../../src/liblare/lare.h"


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
                const generateICs_t genics);


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
local_writeGadgetFile(generateICs_t     genics,
                      int               file,
                      const partBunch_t particles,
                      g9pICMap_t map);


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
	uint32_t minlev = g9pMask_getMinLevel(genics->mask);
	uint32_t maxlev = g9pMask_getMaxLevel(genics->mask);
	uint32_t numFiles = genics->out->numFilesForLevel[genics->zoomlevel-minlev];

	g9pICMap_t map = g9pICMap_new( numFiles, 0, NULL,
	                               g9pMask_getRef(genics->mask),
	                               genics->zoomlevel);

	if (genics->rank == 0)
		generateICs_printSummary(genics, stdout);

	const uint32_t    tmp      = g9pMask_getDim1D(genics->mask);
	gridPointUint32_t fullDims = {tmp, tmp, tmp};
	generateICsOut_initBaseHeader(genics->out, genics->data, fullDims,
	                              genics->mode);

        uint32_t N1, N2, Nperproc;
        uint32_t  NDO=0;
        uint32_t foffset=0;
        for (uint32_t i = 0; i < genics->zoomlevel-minlev; i++){
			foffset+=genics->out->numFilesForLevel[i];
		}

        if (genics->size <=  numFiles ) 
          {
            Nperproc= numFiles / genics->size;
            N1 = Nperproc * genics->rank ;
            N2= Nperproc  + N1;

            if (genics->rank  ==  genics->size -1 )
              {
                N2 = numFiles;
              } 
          }
        else
          {
            if ( genics->rank <=  numFiles -1 )
              {
                N1 = genics->rank;
                N2 = N1+1;
              }
            else
              {NDO=1;}
          }

        printf(" * Comienzo a producir files desde  %i a %i\n", N1, N2);

        if ( NDO == 0 ) 
          {
            for (uint32_t i = N1; i < N2; i++) 
              {
		
		//	for (uint32_t i = 0; i < genics->out->numFiles; i++) {
		printf(" * Working on file %i\n", i+foffset);
		double timing = timer_start();
		
		local_doFile(genics, map, i);
		
		timing = timer_stop(timing);
		printf("      File processed in in %.2fs\n", timing);
	      }
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
	genics->typeForLevel = NULL;
} // local_init

static uint64_t
local_computeNumParts(const generateICs_t genics, uint32_t tile)
{
	uint64_t np;

	np = g9pMask_getNumCellsInTileForLevel( genics->mask, tile,
											genics->zoomlevel);
	                                        //g9pMask_getMinLevel(genics->mask) );

	return np;
}

static uint64_t
local_computeNumPartsLevel(const generateICs_t genics, 
														int8_t level)
{
	uint64_t np=0;
	//int			file = level - g9pMask_getMinLevel(genics->mask);
	uint32_t    firstTile = 0;
	uint32_t    lastTile  = g9pMask_getTotalNumTiles(genics->mask);
	
	
	for(uint32_t tile=firstTile; tile<lastTile; tile++){
		np += g9pMask_getNumCellsInTileForLevel( genics->mask, tile,
	                                        level );
	}

	return np;
}

static void
local_setupCore(generateICsCore_t   core,
                const generateICs_t genics)
{
	core->fullDims[0] = g9pMask_getDim1DLevel(genics->mask,genics->zoomlevel);
	core->fullDims[1] = core->fullDims[0];
	core->fullDims[2] = core->fullDims[0];

	printf("   fullDims = (%u, %u, %u)\n", core->fullDims[0],
	       core->fullDims[1], core->fullDims[2]);
}

static partBunch_t
local_getParticleStorage(const generateICs_t genics,
                         const uint32_t      firstTile,
                         const uint32_t      lastTile)
{
	uint64_t numParticles = UINT64_C(0);
	for (uint32_t i = firstTile; i <= lastTile; i++) {
		numParticles += local_computeNumParts(genics, i);
	}
	if (genics->mode->doGas)
		numParticles *= 2;

	dataVar_t      var;
	dataParticle_t desc = dataParticle_new("Standard", 0, 3);
	var = dataVar_new("Position", DATAVARTYPE_FPV, NDIM);
	(void)dataParticle_addVar(desc, var);
	var = dataVar_new("Velocity", DATAVARTYPE_FPV, NDIM);
	(void)dataParticle_addVar(desc, var);
	if (genics->mode->useLongIDs)
		var = dataVar_new("ID", DATAVARTYPE_INT64, 1);
	else
		var = dataVar_new("ID", DATAVARTYPE_INT32, 1);
	(void)dataParticle_addVar(desc, var);
	dataParticle_lock(desc);

	partBunch_t particles = partBunch_new(desc, numParticles);
	partBunch_allocMem(particles);

	return particles;
} // local_getParticleStorage

static void
local_doFile(generateICs_t genics, g9pICMap_t map, int file)
{
	uint32_t    firstTile = g9pICMap_getFirstTileInFile(map, file);
	uint32_t    lastTile  = g9pICMap_getLastTileInFile(map, file);
	uint64_t    partsRead = UINT64_C(0);

	partBunch_t particles = local_getParticleStorage(genics,
	                                                 firstTile, lastTile);

	generateICsCore_s core = GENICSCORE_INIT_STRUCT(genics->data,
	                                                genics->mode);
	local_setupCore(&core, genics);
	
	printf("np in level: %i\n",
				local_computeNumPartsLevel(genics, genics->zoomlevel)); 
	
	for (uint32_t i = firstTile; i <= lastTile; i++) {
		printf("cells in tile: %i\n", g9pMask_getNumCellsInTileForLevel(genics->mask,i,genics->zoomlevel));
		core.numParticles = local_computeNumParts(genics, i);
		if(core.numParticles > 0) {
		core.pos          = partBunch_at(particles, 0, partsRead);
		core.vel          = partBunch_at(particles, 1, partsRead);
		core.id           = partBunch_at(particles, 2, partsRead);
		core.patch        = g9pMask_getEmptyPatchForTileLevel(genics->mask, i, genics->zoomlevel);
		core.level		  = genics->zoomlevel;
		core.maxDims	  = g9pMask_getDim1DLevel(genics->mask,g9pMask_getMaxLevel(genics->mask));
		core.maskdata	  = g9pMask_getTileData(genics->mask,i);
		core.maskDim1D	  = g9pMask_getDim1D(genics->mask);
		core.partDim1D	  = g9pMask_getDim1DLevel(genics->mask,genics->zoomlevel);
		(void)gridPatch_attachVar(core.patch, genics->in->varVelx);
		(void)gridPatch_attachVar(core.patch, genics->in->varVely);
		(void)gridPatch_attachVar(core.patch, genics->in->varVelz);

//		fpv_t             *velxP = gridPatch_getVarDataHandle(core.patch, 0);
	//	printf("\n %i \n", i);

		gridReader_readIntoPatchForVar(genics->in->velx, core.patch, 0);
		gridReader_readIntoPatchForVar(genics->in->vely, core.patch, 1);
		gridReader_readIntoPatchForVar(genics->in->velz, core.patch, 2);
		
		//fpv_t             *velx1P = gridPatch_getVarDataHandle(core.patch, 0);
		//printf(" %i \n", velx1P);
		
		generateICsCore_toParticles(&core);

		//fpv_t             *velx2P = gridPatch_getVarDataHandle(core.patch, 0);
		//printf(" %i \n\n", velx2P);

		gridPatch_del( &(core.patch) );
		partsRead += core.numParticles;
	    }
	}
	printf("   Particles read: %lu\n", partsRead);
	//printf("pos: %f", core.pos[1]);
	if (genics->mode->doGas) {
		uint64_t npGasTotal = local_computeNumPartsLevel(genics, genics->zoomlevel);
		//npGasTotal = core.fullDims[0];
		//npGasTotal *= core.fullDims[1];
		//npGasTotal *= core.fullDims[2];
		core.numParticles = partBunch_getNumParticles(particles);
		core.pos          = partBunch_at(particles, 0, 0);
		core.vel          = partBunch_at(particles, 1, 0);
		core.id           = partBunch_at(particles, 2, 0);
		generateICsCode_dm2Gas(&core, 0.25, npGasTotal);
	}
	
	if (genics->mode->autoCenter) {
		float newCenter[3];
		g9pMask_getCenter(genics->mask, newCenter);
		core.numParticles = partBunch_getNumParticles(particles);
		core.pos          = partBunch_at(particles, 0, 0);
		core.vel          = partBunch_at(particles, 1, 0);
		core.id           = partBunch_at(particles, 2, 0);
		generateICsCore_recenter(&core, newCenter);
	}

	local_writeGadgetFile(genics, file, particles, map);

	partBunch_del(&particles);
} // local_doFile

static void
local_writeGadgetFile(generateICs_t     genics,
                      int               file,
                      const partBunch_t particles,
                      g9pICMap_t map)
{
	uint32_t       npLocal[6] = {0, 0, 0, 0, 0, 0};
	uint64_t       npAll[6] = {0, 0, 0, 0, 0, 0};
	double         massArr[6] = {0., 0., 0., 0., 0., 0.};
	gadgetHeader_t myHeader;

	const uint64_t               np = partBunch_getNumParticles(particles);
	uint32_t minlev = g9pMask_getMinLevel(genics->mask);
	uint32_t maxlev = g9pMask_getMaxLevel(genics->mask);
	uint32_t   numLevels = maxlev - minlev + 1;
	uint32_t   arrIdx = (genics->typeForLevel)[genics->zoomlevel-minlev];
	uint32_t nlevfortype[6] = {0, 0, 0, 0, 0, 0};
	uint64_t npFull;
	
	uint32_t foffset=0;
        for (uint32_t i = 0; i < genics->zoomlevel-minlev; i++){
			foffset+=genics->out->numFilesForLevel[i];
		}
	
    for(int lev=minlev; lev<=maxlev; lev++) {
		for(int type=0; type<=6; type++) {
			if((genics->typeForLevel)[lev-minlev]==type) nlevfortype[type]++;
		}
	}
    
	if (genics->mode->doGas) {
		assert(np % 2 == 0);
		npLocal[0] = np / 2;
		npLocal[arrIdx] = npLocal[0];
	} else {
		npLocal[arrIdx] = (uint32_t)np;
	}
	
	myHeader   = gadgetHeader_clone(genics->out->baseHeader);
	
	// set global header for zoom:
	uint32_t idx;
	for(int level = minlev; level<= maxlev; level++) {
		idx = (genics->typeForLevel)[level-minlev];
		npAll[idx] += (uint64_t)local_computeNumPartsLevel(genics, level);
		if(nlevfortype[idx]==1) {
			npFull = POW_NDIM((uint64_t)g9pMask_getDim1DLevel(genics->mask,level));
			massArr[idx] = generateICsOut_boxMass(genics->data) / npFull;
		} else {
			massArr[idx] = 0;
		}
		
		//idx = 1 + maxlev - level;
		//npAll[idx] = (uint64_t)local_computeNumPartsFileLevel(genics, map, level);
		//npFull = POW_NDIM(g9pMask_getDim1DLevel(genics->mask,level));
		//massArr[idx] = generateICsOut_boxMass(genics->data) / npFull;
	}
	printf("\n mass: %lf\n",generateICsOut_boxMass(genics->data));
	
	if(nlevfortype[arrIdx]>1) {
		gadgetTOC_addEntryByType(genics->out->toc, GADGETBLOCK_MASS);
	}
	if (genics->mode->doGas) {
		const double omegaBaryon0 = cosmoModel_getOmegaBaryon0(genics->data->model);
		const double omegaMatter0 = cosmoModel_getOmegaMatter0(genics->data->model);
		npAll[0]    = npAll[arrIdx];
		massArr[0]  = massArr[arrIdx] * omegaBaryon0 / omegaMatter0;
		massArr[arrIdx] -= massArr[0];
		gadgetTOC_addEntryByType(genics->out->toc, GADGETBLOCK_U___);
	}
	gadgetHeader_setNall(myHeader,npAll);
	gadgetHeader_setMassArr(myHeader, massArr);
	//gadgetHeader_setNumFiles(myHeader,numLevels);
	//
	
	gadgetHeader_getMassArr(myHeader, massArr);
	gadgetHeader_setNp(myHeader, npLocal);
	gadgetTOC_calcSizes(genics->out->toc, npLocal, massArr, false,
	                    genics->mode->useLongIDs);
	gadgetTOC_calcOffset(genics->out->toc);
	gadget_setHeaderOfFile(genics->out->gadget, file+foffset, myHeader);
	gadget_setTOCOfFile( genics->out->gadget, file+foffset,
	                     gadgetTOC_clone(genics->out->toc) );
	gadget_open(genics->out->gadget, GADGET_MODE_WRITE_CREATE, file+foffset);
	gadget_writeHeaderToCurrentFile(genics->out->gadget);
	{
		stai_t stai;
		stai = stai_new( partBunch_at(particles, 0, 0),
		                 3 * sizeof(fpv_t), 3 * sizeof(fpv_t) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_POS_,
		                               0, np, stai);
		stai_del(&stai);
		stai = stai_new( partBunch_at(particles, 1, 0),
		                 3 * sizeof(fpv_t), 3 * sizeof(fpv_t) );
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_VEL_,
		                               0, np, stai);
		stai_del(&stai);
		if (genics->mode->useLongIDs) {
			stai = stai_new( partBunch_at(particles, 2, 0),
			                 sizeof(uint64_t), sizeof(uint64_t) );
		} else {
			stai = stai_new( partBunch_at(particles, 2, 0),
			                 sizeof(uint32_t), sizeof(uint32_t) );
		}
		gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_ID__,
		                               0, np, stai);
		stai_del(&stai);
		
		
		if(nlevfortype[arrIdx]>1) {
			fpv_t masses[np];
			npFull = POW_NDIM(g9pMask_getDim1DLevel(genics->mask,genics->zoomlevel));
			fpv_t mass1 = generateICsOut_boxMass(genics->data) / npFull;
			for(int i=0; i<np; i++) {
				masses[i] = mass1;
			}
			stai = stai_new( masses, sizeof(fpv_t), sizeof(fpv_t) );
			gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_MASS,
		                               0, np, stai);
		    stai_del(&stai);
		}
		
		if(genics->mode->doGas) {
			uint64_t npl = npLocal[0];
			fpv_t energies[npl];
			for(int i=0;i<npl;i++) {
				energies[i]=0;
			}
			stai = stai_new(energies, sizeof(fpv_t), sizeof(fpv_t) );
			gadget_writeBlockToCurrentFile(genics->out->gadget, GADGETBLOCK_U___,
		                               0, npl, stai);
		    stai_del(&stai);
		}
	}
	gadget_close(genics->out->gadget);
} // local_writeGadgetFile
