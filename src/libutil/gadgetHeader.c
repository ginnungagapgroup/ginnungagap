// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetHeader.h"
#include "gadgetUtil.h"
#include <assert.h>
#include <string.h>
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gadgetHeader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


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
	gadgetHeader->expansion       = 1.0;
	gadgetHeader->redshift        = 0.0;
	gadgetHeader->flagsfr         = 0;
	gadgetHeader->flagfeedback    = 0;
	gadgetHeader->flagcooling     = 0;
	gadgetHeader->numfiles        = 1;
	gadgetHeader->boxsize         = 0.0;
	gadgetHeader->omega0          = 0.0;
	gadgetHeader->omegalambda     = 0.0;
	gadgetHeader->hubbleparameter = 0.0;
	gadgetHeader->flagstellarage  = 0;
	gadgetHeader->flagmetal       = 0;
	gadgetHeader->flagentropy     = 0;

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
gadgetHeader_setNp(gadgetHeader_t gadgetHeader, uint32_t np[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++)
		gadgetHeader->np[i] = np[i];
}

extern void
gadgetHeader_setMassArr(gadgetHeader_t gadgetHeader, double massarr[6])
{
	assert(gadgetHeader != NULL);

	for (int i = 0; i < 6; i++)
		gadgetHeader->massarr[i] = massarr[i];
}

extern void
gadgetHeader_setExpansion(gadgetHeader_t gadgetHeader, double expansion)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->expansion = expansion;
	gadgetHeader->redshift  = 1. / (gadgetHeader->expansion) - 1;
}

extern void
gadgetHeader_setRedshift(gadgetHeader_t gadgetHeader, double redshift)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->redshift  = redshift;
	gadgetHeader->expansion = 1. / (gadgetHeader->redshift + 1);
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
gadgetHeader_setNall(gadgetHeader_t gadgetHeader, uint64_t nall[6])
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

	gadgetHeader->numfiles = numFiles;
}

extern void
gadgetHeader_setBoxsize(gadgetHeader_t gadgetHeader, double boxsize)
{
	assert(gadgetHeader != NULL);

	gadgetHeader->boxsize = boxsize;
}

extern void
gadgetHeader_setOmega0(gadgetHeader_t gadgetHeader, double omega0)
{
	assert(gadgetHeader != NULL);

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
gadgetHeader_write(gadgetHeader_t gadgetHeader, FILE *f)
{
	const static uint32_t thisBlockSize = GADGETHEADER_SIZE;
	uint8_t               fill[GADGETHEADER_SIZE_UNUSED];

	assert(gadgetHeader != NULL);
	assert(f != NULL);

	for (int i = 0; i < GADGETHEADER_SIZE_UNUSED; i++)
		fill[i] = 0;

	gadgetUtil_writeBlock(f, "HEAD", GADGETHEADER_SIZE);

	xfwrite(&thisBlockSize, sizeof(uint32_t), 1, f);

	xfwrite(gadgetHeader->np, sizeof(uint32_t), 6, f);
	xfwrite(gadgetHeader->massarr, sizeof(double), 6, f);
	xfwrite(&(gadgetHeader->expansion), sizeof(double), 1, f);
	xfwrite(&(gadgetHeader->redshift), sizeof(double), 1, f);
	xfwrite(&(gadgetHeader->flagsfr), sizeof(int32_t), 1, f);
	xfwrite(&(gadgetHeader->flagfeedback), sizeof(int32_t), 1, f);
	xfwrite(gadgetHeader->nall, sizeof(uint32_t), 6, f);
	xfwrite(&(gadgetHeader->flagcooling), sizeof(int32_t), 1, f);
	xfwrite(&(gadgetHeader->numfiles), sizeof(int32_t), 1, f);
	xfwrite(&(gadgetHeader->boxsize), sizeof(double), 1, f);
	xfwrite(&(gadgetHeader->omega0), sizeof(double), 1, f);
	xfwrite(&(gadgetHeader->omegalambda), sizeof(double), 1, f);
	xfwrite(&(gadgetHeader->hubbleparameter), sizeof(double), 1, f);
	xfwrite(&(gadgetHeader->flagstellarage), sizeof(int32_t), 1, f);
	xfwrite(&(gadgetHeader->flagmetal), sizeof(int32_t), 1, f);
	xfwrite(gadgetHeader->nallhighw, sizeof(uint32_t), 6, f);
	xfwrite(&(gadgetHeader->flagentropy), sizeof(int32_t), 1, f);

	xfwrite(fill, sizeof(int8_t), GADGETHEADER_SIZE_UNUSED, f);
	xfwrite(&thisBlockSize, sizeof(uint32_t), 1, f);
} /* gadgetHeader_write */

/*--- Implementations of local functions --------------------------------*/
