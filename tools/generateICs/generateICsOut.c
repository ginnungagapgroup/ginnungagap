// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsOut.c
 * @ingroup  toolsGICSOut
 * @brief  Implements the output wrapper object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICsOut.h"
#include <assert.h>
#include "../../src/libcosmo/cosmo.h"
#include "../../src/libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_setConst(generateICsOut_t out, int numFiles);


/*--- Implementations of exported functions -----------------------------*/
extern generateICsOut_t
generateICsOut_new(const char *prefix, int outputFiles, gadgetVersion_t v)
{
	generateICsOut_t genicsOut;

	genicsOut = xmalloc( sizeof(struct generateICsOut_struct) );

	local_setConst(genicsOut, outputFiles);

	genicsOut->gadget = gadget_newSimple(prefix, outputFiles);
	gadget_setFileVersion(genicsOut->gadget, v);

	genicsOut->toc = gadgetTOC_new();
	gadgetTOC_addEntryByType(genicsOut->toc, GADGETBLOCK_HEAD);
	gadgetTOC_addEntryByType(genicsOut->toc, GADGETBLOCK_POS_);
	gadgetTOC_addEntryByType(genicsOut->toc, GADGETBLOCK_VEL_);
	gadgetTOC_addEntryByType(genicsOut->toc, GADGETBLOCK_ID__);

	genicsOut->baseHeader = NULL;

	return genicsOut;
} // generateICsOut_new

extern void
generateICsOut_del(generateICsOut_t *genicsOut)
{
	assert(genicsOut != NULL && *genicsOut != NULL);

	gadgetTOC_del( &( (*genicsOut)->toc ) );
	if ( (*genicsOut)->baseHeader != NULL ) {
		gadgetHeader_del( &( (*genicsOut)->baseHeader ) );
	}
	gadget_del( &( (*genicsOut)->gadget ) );

	xfree(*genicsOut);

	*genicsOut = NULL;
}

extern void
generateICsOut_initBaseHeader(generateICsOut_t        genicsOut,
                              const generateICsData_t data,
                              const uint32_t          np[3],
                              const generateICsMode_t mode)
{
	uint64_t       npall[6];
	double         massarr[6];
	const double   boxsize      = data->boxsizeInMpch;
	const double   omegaMatter0 = cosmoModel_getOmegaMatter0(data->model);
	const int      numFiles     = gadget_getNumFiles(genicsOut->gadget);

	gadgetHeader_t header       = gadgetHeader_new();

	for (int i = 0; i < 6; i++) {
		npall[i]   = 0;
		massarr[i] = 0;
	}

	npall[1]    = (uint64_t)(np[0]) * (uint64_t)(np[1]) * (uint64_t)(np[2]);
	massarr[1]  = boxsize * boxsize * boxsize * omegaMatter0 / (npall[1]);
	massarr[1] *= COSMO_RHO_CRIT0 * 1e-10;

	if (mode->doGas) {
		const double omegaBaryon0 = cosmoModel_getOmegaBaryon0(data->model);
		npall[0]    = npall[1];
		massarr[0]  = massarr[1] * omegaBaryon0 / omegaMatter0;
		massarr[1] -= massarr[0];
	}

	gadgetHeader_setMassArr(header, massarr);
	gadgetHeader_setTime(header, data->aInit);
	gadgetHeader_setRedshift( header, cosmo_a2z(data->aInit) );
	gadgetHeader_setNall(header, npall);
	gadgetHeader_setNumFiles(header, numFiles);
	gadgetHeader_setBoxsize(header, boxsize * data->posFactor);
	gadgetHeader_setOmega0( header, cosmoModel_getOmegaMatter0(data->model) );
	gadgetHeader_setOmegaLambda( header,
	                             cosmoModel_getOmegaLambda0(data->model) );
	gadgetHeader_setHubbleParameter( header,
	                                 cosmoModel_getSmallH(data->model) );

	gadgetHeader_setUseLongIDs(header, mode->useLongIDs);

	if (genicsOut->baseHeader != NULL) {
		gadgetHeader_del( &(genicsOut->baseHeader) );
	}
	genicsOut->baseHeader = header;
} // generateICsOut_initBaseHeader

/*--- Implementations of local functions --------------------------------*/
static void
local_setConst(generateICsOut_t out, int numFiles)
{
	*(int *)&(out->numFiles) = numFiles;
}
