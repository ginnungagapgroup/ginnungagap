// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetHeader.c
 * @ingroup libutilFilesGadget
 * @brief  This file provides the implementation of the Gadget header
 *         object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetHeader.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gadgetHeader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_actualWriteHeader(const gadgetHeader_t header, FILE *f);

static void
local_actualReadHeader(gadgetHeader_t header, FILE *f);


/*--- Implementations of exported functios ------------------------------*/
extern gadgetHeader_t
gadgetHeader_new(void)
{
	gadgetHeader_t gadgetHeader;

	gadgetHeader = xmalloc(sizeof(struct gadgetHeader_struct));

	for (int i = 0; i < 6; i++) {
		gadgetHeader->np[i]        = 0;
		gadgetHeader->massarr[i]   = 0.0;
		gadgetHeader->nall[i]      = 0;
		gadgetHeader->nallhighw[i] = 0;
	}
	gadgetHeader->time                = 1.0;
	gadgetHeader->redshift            = 0.0;
	gadgetHeader->flagsfr             = 0;
	gadgetHeader->flagfeedback        = 0;
	gadgetHeader->flagcooling         = 0;
	gadgetHeader->numfiles            = 1;
	gadgetHeader->boxsize             = 0.0;
	gadgetHeader->omega0              = 0.0;
	gadgetHeader->omegalambda         = 0.0;
	gadgetHeader->hubbleparameter     = 0.0;
	gadgetHeader->flagstellarage      = 0;
	gadgetHeader->flagmetal           = 0;
	gadgetHeader->flagentropy         = 0;
	gadgetHeader->flagdoubleprecision = 0;
	gadgetHeader->flagicinfo          = 0;
	gadgetHeader->lptscalingfactor    = 0.0f;

	return gadgetHeader;
}

extern gadgetHeader_t
gadgetHeader_clone(const gadgetHeader_t gadgetHeader)
{
	gadgetHeader_t clone;

	assert(gadgetHeader != NULL);

	clone = xmalloc(sizeof(struct gadgetHeader_struct));
	memcpy(clone, gadgetHeader, sizeof(struct gadgetHeader_struct));

	return clone;
}

extern void
gadgetHeader_del(gadgetHeader_t *gadgetHeader)
{
	assert(gadgetHeader != NULL && *gadgetHeader != NULL);

	xfree(*gadgetHeader);

	*gadgetHeader = NULL;
}

extern void
gadgetHeader_setNp(gadgetHeader_t gadgetHeader, const uint32_t np[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++)
		gadgetHeader->np[i] = np[i];
}

extern void
gadgetHeader_setMassArr(gadgetHeader_t gadgetHeader,
                        const double   massarr[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++) {
		assert(isgreaterequal(massarr[i], 0.0));
		gadgetHeader->massarr[i] = massarr[i];
	}
}

extern void
gadgetHeader_setTime(gadgetHeader_t gadgetHeader, double time)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->time = time;
}

extern void
gadgetHeader_setRedshift(gadgetHeader_t gadgetHeader, double redshift)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->redshift = redshift;
}

extern void
gadgetHeader_setFlagSfr(gadgetHeader_t gadgetHeader, int32_t flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagsfr = flag;
}

extern void
gadgetHeader_setFlagFeedback(gadgetHeader_t gadgetHeader, int32_t flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagfeedback = flag;
}

extern void
gadgetHeader_setNall(gadgetHeader_t gadgetHeader, const uint64_t nall[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++) {
		gadgetHeader->nall[i]      = (uint32_t)nall[i];
		gadgetHeader->nallhighw[i] = (uint32_t)(nall[i] >> 32);
	}
}

extern void
gadgetHeader_setFlagCooling(gadgetHeader_t gadgetHeader, int32_t flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagcooling = flag;
}

extern void
gadgetHeader_setNumFiles(gadgetHeader_t gadgetHeader, int32_t numFiles)
{
	assert(gadgetHeader != NULL);
	assert(numFiles > 0);

	gadgetHeader->numfiles = numFiles;
}

extern void
gadgetHeader_setBoxsize(gadgetHeader_t gadgetHeader, double boxsize)
{
	assert(gadgetHeader != NULL);
	assert(isgreaterequal(boxsize, 0.0));

	gadgetHeader->boxsize = boxsize;
}

extern void
gadgetHeader_setOmega0(gadgetHeader_t gadgetHeader, double omega0)
{
	assert(gadgetHeader != NULL);
	assert(isgreaterequal(omega0, 0.0));

	gadgetHeader->omega0 = omega0;
}

extern void
gadgetHeader_setOmegaLambda(gadgetHeader_t gadgetHeader, double omegaLambda)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->omegalambda = omegaLambda;
}

extern void
gadgetHeader_setHubbleParameter(gadgetHeader_t gadgetHeader, double h)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->hubbleparameter = h;
}

extern void
gadgetHeader_setFlagStellarAge(gadgetHeader_t gadgetHeader, int32_t flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagstellarage = flag;
}

extern void
gadgetHeader_setFlagMetal(gadgetHeader_t gadgetHeader, int32_t flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagmetal = flag;
}

extern void
gadgetHeader_setFlagEntropy(gadgetHeader_t gadgetHeader, int32_t flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagentropy = flag;
}

extern void
gadgetHeader_setFlagDoublePrecision(gadgetHeader_t gadgetHeader,
                                    int32_t        flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagdoubleprecision = flag;
}

extern void
gadgetHeader_setFlagICInfo(gadgetHeader_t gadgetHeader, int32_t flag)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->flagicinfo = flag;
}

extern void
gadgetHeader_setLPTScalingFactor(gadgetHeader_t gadgetHeader,
                                 float          lptScalingFactor)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->lptscalingfactor = lptScalingFactor;
}

extern void
gadgetHeader_getNp(const gadgetHeader_t gadgetHeader, uint32_t np[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++)
		np[i] = gadgetHeader->np[i];
}

extern void
gadgetHeader_getMassArr(const gadgetHeader_t gadgetHeader,
                        double               massarr[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++)
		massarr[i] = gadgetHeader->massarr[i];
}

extern double
gadgetHeader_getTime(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->time;
}

extern double
gadgetHeader_getRedshift(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->redshift;
}

extern int32_t
gadgetHeader_getFlagSfr(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagsfr;
}

extern int32_t
gadgetHeader_getFlagFeedback(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagfeedback;
}

extern void
gadgetHeader_getNall(const gadgetHeader_t gadgetHeader, uint64_t nall[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++) {
		nall[i]   = gadgetHeader->nallhighw[i];
		nall[i] <<= 32;
		nall[i]  += gadgetHeader->nall[i];
	}
}

extern int32_t
gadgetHeader_getFlagCooling(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagcooling;
}

extern int32_t
gadgetHeader_getNumFiles(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->numfiles;
}

extern double
gadgetHeader_getBoxsize(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->boxsize;
}

extern double
gadgetHeader_getOmega0(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->omega0;
}

extern double
gadgetHeader_getOmegaLambda(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->omegalambda;
}

extern double
gadgetHeader_getHubbleParameter(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->hubbleparameter;
}

extern int32_t
gadgetHeader_getFlagStellarAge(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagstellarage;
}

extern int32_t
gadgetHeader_getFlagMetal(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagmetal;
}

extern int32_t
gadgetHeader_getFlagEntropy(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagentropy;
}

extern int32_t
gadgetHeader_getFlagDoublePrecision(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagdoubleprecision;
}

extern int32_t
gadgetHeader_getFlagICInfo(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->flagicinfo;
}

extern float
gadgetHeader_getLPTScalingFactor(const gadgetHeader_t gadgetHeader)
{
	assert(gadgetHeader != NULL);

	return gadgetHeader->lptscalingfactor;
}

extern uint64_t
gadgetHeader_getNumPartsInFile(const gadgetHeader_t gadgetHeader)
{
	uint64_t numPartsInFile = 0;

	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++)
		numPartsInFile += gadgetHeader->np[i];

	return numPartsInFile;
}

extern void
gadgetHeader_write(const gadgetHeader_t gadgetHeader, FILE *f)
{
	static const uint32_t thisBlockSize = GADGETHEADER_SIZE;

	assert(gadgetHeader != NULL);
	assert(f != NULL);

	xfwrite(&thisBlockSize, sizeof(uint32_t), 1, f);
	local_actualWriteHeader(gadgetHeader, f);
	xfwrite(&thisBlockSize, sizeof(uint32_t), 1, f);
}

extern void
gadgetHeader_read(gadgetHeader_t gadgetHeader, FILE *f)
{
	uint32_t blockSize1, blockSize2;

	assert(gadgetHeader != NULL);
	assert(f != NULL);

	xfread(&blockSize1, sizeof(uint32_t), 1, f);
	local_actualReadHeader(gadgetHeader, f);
	xfread(&blockSize2, sizeof(uint32_t), 1, f);

	if (blockSize1 != blockSize2) {
		fprintf(stderr,
		        "Error in %s: Blocksizes don't match (%" PRIu32
		        " != %" PRIu32 ")\n",
		        __func__, blockSize1, blockSize2);
		exit(EXIT_FAILURE);
	}
}

/*--- Implementations of local functions --------------------------------*/
static void
local_actualWriteHeader(const gadgetHeader_t header, FILE *f)
{
	uint8_t fill[GADGETHEADER_SIZE_UNUSED];

	for (int i = 0; i < GADGETHEADER_SIZE_UNUSED; i++)
		fill[i] = 0;

	xfwrite(header->np, sizeof(uint32_t), 6, f);
	xfwrite(header->massarr, sizeof(double), 6, f);
	xfwrite(&(header->time), sizeof(double), 1, f);
	xfwrite(&(header->redshift), sizeof(double), 1, f);
	xfwrite(&(header->flagsfr), sizeof(int32_t), 1, f);
	xfwrite(&(header->flagfeedback), sizeof(int32_t), 1, f);
	xfwrite(header->nall, sizeof(uint32_t), 6, f);
	xfwrite(&(header->flagcooling), sizeof(int32_t), 1, f);
	xfwrite(&(header->numfiles), sizeof(int32_t), 1, f);
	xfwrite(&(header->boxsize), sizeof(double), 1, f);
	xfwrite(&(header->omega0), sizeof(double), 1, f);
	xfwrite(&(header->omegalambda), sizeof(double), 1, f);
	xfwrite(&(header->hubbleparameter), sizeof(double), 1, f);
	xfwrite(&(header->flagstellarage), sizeof(int32_t), 1, f);
	xfwrite(&(header->flagmetal), sizeof(int32_t), 1, f);
	xfwrite(header->nallhighw, sizeof(uint32_t), 6, f);
	xfwrite(&(header->flagentropy), sizeof(int32_t), 1, f);
	xfwrite(&(header->flagdoubleprecision), sizeof(int32_t), 1, f);
	xfwrite(&(header->flagicinfo), sizeof(int32_t), 1, f);
	xfwrite(&(header->lptscalingfactor), sizeof(float), 1, f);
	xfwrite(fill, sizeof(int8_t), GADGETHEADER_SIZE_UNUSED, f);
}

static void
local_actualReadHeader(gadgetHeader_t header, FILE *f)
{
	uint8_t fill[GADGETHEADER_SIZE_UNUSED];

	xfread(header->np, sizeof(uint32_t), 6, f);
	xfread(header->massarr, sizeof(double), 6, f);
	xfread(&(header->time), sizeof(double), 1, f);
	xfread(&(header->redshift), sizeof(double), 1, f);
	xfread(&(header->flagsfr), sizeof(int32_t), 1, f);
	xfread(&(header->flagfeedback), sizeof(int32_t), 1, f);
	xfread(header->nall, sizeof(uint32_t), 6, f);
	xfread(&(header->flagcooling), sizeof(int32_t), 1, f);
	xfread(&(header->numfiles), sizeof(int32_t), 1, f);
	xfread(&(header->boxsize), sizeof(double), 1, f);
	xfread(&(header->omega0), sizeof(double), 1, f);
	xfread(&(header->omegalambda), sizeof(double), 1, f);
	xfread(&(header->hubbleparameter), sizeof(double), 1, f);
	xfread(&(header->flagstellarage), sizeof(int32_t), 1, f);
	xfread(&(header->flagmetal), sizeof(int32_t), 1, f);
	xfread(header->nallhighw, sizeof(uint32_t), 6, f);
	xfread(&(header->flagentropy), sizeof(int32_t), 1, f);
	xfread(&(header->flagdoubleprecision), sizeof(int32_t), 1, f);
	xfread(&(header->flagicinfo), sizeof(int32_t), 1, f);
	xfread(&(header->lptscalingfactor), sizeof(float), 1, f);
	xfread(fill, sizeof(int8_t), GADGETHEADER_SIZE_UNUSED, f);
}
