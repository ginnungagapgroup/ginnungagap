// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/artHeader.c
 * @ingroup libutilFilesART
 * @brief  This file provides the implementation of the ART file header.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "artHeader.h"
#include <assert.h>
#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "xmem.h"
#include "xfile.h"
#include "endian.h"
#include "byteswap.h"
#include "diediedie.h"
#include "utilMath.h"


/*--- Implementation of main structure ----------------------------------*/
#include "artHeader_adt.h"


/*--- Local defines -----------------------------------------------------*/

/**
 * @brief  Gives the critical density in (M_sun/h)/(Mpc/h)^3
 * @todo  This really should not be here but in a proper cosmology
 *        header.
 */
#define RHO_CRIT_0 2.7755397e11


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Set all fields of a header structure to 'null' values.
 *
 * @param[in]  header
 *                The header to null, passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_nullHeader(artHeader_t header);


/**
 * @brief  This will perform the actual read from the file.
 *
 * @param[in,out]  header
 *                    The header structure to fill.
 * @param[in,out]  *f
 *                    The file from which to read.
 *
 * @return  Returns nothing.
 */
static void
local_readHeaderActual(artHeader_t header, FILE *f);


/**
 * @brief  Will byteswap all values in the header.
 *
 * @param[in,out]  header
 *                    The header to be byteswapped.
 *
 * @return  Returns nothing.
 */
static void
local_byteswapHeader(artHeader_t header);


/**
 * @brief  This calculates the scaling factor between file internal
 *         particle weights and actual masses in M_sun/h.
 *
 * This should be called every time the box-size,
 * artHeader_struct::ngridc or artHeader_struct::Om0 changes.
 *
 * @param[in,out]  header
 *                    The header for which to update the scaling factor.
 *
 * @return  Returns nothing.
 */
static void
local_calcFactorWeight(artHeader_t header);


/**
 * @brief  This calculates the scaling factor between file internal
 *         particle positions and actual positions in Mpc/h.
 *
 * This should be called every time the box-size or @c ngridc changes.
 *
 * @param[in,out]  header
 *                    The header for which to update the scaling factor.
 *
 * @return  Returns nothing.
 */
static void
local_calcFactorPosition(artHeader_t header);


/**
 * @brief  This calculates the scaling factor between file internal
 *          velocities and proper velocities in km/s.
 *
 * This should be called every time artHeader_struct::ngridc,
 * artHeader_struct::aexpn or the boxsize changes.
 *
 * @param[in,out]  header
 *                    The header for which to update the scaling factor.
 *
 * @return  Returns nothing.
 */
static void
local_calFactorVelocity(artHeader_t header);


/*--- Implementations of exported functions -----------------------------*/
extern artHeader_t
artHeader_new(void)
{
	artHeader_t header;

	header           = xmalloc(sizeof(struct artHeader_struct));
	header->wspecies = header->extras;
	header->lspecies = (int64_t *)(header->extras + 10);

	local_nullHeader(header);

	return header;
}

extern artHeader_t
artHeader_newFromFile(const char *headerFileName)
{
	artHeader_t header;
	assert(headerFileName != NULL);

	header = artHeader_new();
	artHeader_read(header, headerFileName);

	return header;
}

extern void
artHeader_del(artHeader_t *header)
{
	assert(header != NULL && *header != NULL);

	xfree(*header);
	*header = NULL;
}

extern void
artHeader_setHeaderString(artHeader_t header, const char *string)
{
	int len;
	assert(header != NULL);

	len = strlen(string);
	len = (len > ARTHEADER_HEADERSTRING_LENGTH)
	      ? ARTHEADER_HEADERSTRING_LENGTH : len;
	memcpy(header->headerString, string, len);
	for (; len < ARTHEADER_HEADERSTRING_LENGTH + 1; len++)
		header->headerString[len] = '\0';
}

extern void
artHeader_setAexpn(artHeader_t header, float aexpn)
{
	assert(header != NULL);
	assert(isgreater(aexpn, 0.0));

	header->aexpn = aexpn;
	local_calFactorVelocity(header);
}

extern void
artHeader_setAexp0(artHeader_t header, float aexp0)
{
	assert(header != NULL);
	assert(isgreater(aexp0, 0.0));

	header->aexp0 = aexp0;
}

extern void
artHeader_setAmplt(artHeader_t header, float amplt)
{
	assert(header != NULL);
	assert(isgreater(amplt, 0.0));

	header->amplt = amplt;
}

extern void
artHeader_setAstep(artHeader_t header, float astep)
{
	assert(header != NULL);
	assert(isgreater(astep, 0.0));

	header->astep = astep;
}

extern void
artHeader_setIstep(artHeader_t header, int istep)
{
	assert(header != NULL);
	assert(istep >= 0);

	header->istep = istep;
}

extern void
artHeader_setPartw(artHeader_t header, float partw)
{
	assert(header != NULL);

	header->partw = partw;
}

extern void
artHeader_setTintg(artHeader_t header, float tintg)
{
	assert(header != NULL);

	header->tintg = tintg;
}

extern void
artHeader_setEkin(artHeader_t header, float ekin)
{
	assert(header != NULL);

	header->ekin = ekin;
}

extern void
artHeader_setEkin1(artHeader_t header, float ekin1)
{
	assert(header != NULL);

	header->ekin1 = ekin1;
}

extern void
artHeader_setEkin2(artHeader_t header, float ekin2)
{
	assert(header != NULL);

	header->ekin2 = ekin2;
}

extern void
artHeader_setAu0(artHeader_t header, float au0)
{
	assert(header != NULL);

	header->au0 = au0;
}

extern void
artHeader_setAeu0(artHeader_t header, float aeu0)
{
	assert(header != NULL);

	header->aeu0 = aeu0;
}

extern void
artHeader_setNrowc(artHeader_t header, int nrowc)
{
	assert(header != NULL);
	assert((nrowc > 0) && ((nrowc & (nrowc - 1)) == 0));

	header->nrowc = nrowc;
}

extern void
artHeader_setNgridc(artHeader_t header, int ngridc)
{
	assert(header != NULL);
	assert((ngridc > 0) && ((ngridc & (ngridc - 1)) == 0));

	header->ngridc = ngridc;

	local_calcFactorWeight(header);
	local_calcFactorPosition(header);
	local_calFactorVelocity(header);
}

extern void
artHeader_setNspecies(artHeader_t header, int nspecies)
{
	assert(header != NULL);

	header->nspecies = nspecies;
}

extern void
artHeader_setNseed(artHeader_t header, int nseed)
{
	assert(header != NULL);

	header->nseed = nseed;
}

extern void
artHeader_setOm0(artHeader_t header, float Om0)
{
	assert(header != NULL);

	header->Om0 = Om0;

	local_calcFactorWeight(header);
}

extern void
artHeader_setOml0(artHeader_t header, float Oml0)
{
	assert(header != NULL);

	header->Oml0 = Oml0;
}

extern void
artHeader_setHubble(artHeader_t header, float hubble)
{
	assert(header != NULL);

	header->hubble = hubble;
}

extern void
artHeader_setWp5(artHeader_t header, float wp5)
{
	assert(header != NULL);

	header->wp5 = wp5;
}

extern void
artHeader_setOcurv(artHeader_t header, float Ocurv)
{
	assert(header != NULL);

	header->Ocurv = Ocurv;
}

extern void
artHeader_setExtras(artHeader_t header, float value, int position)
{
	assert(header != NULL);
	assert(position >= 0 && position < 100);

	header->extras[position] = value;
}

extern void
artHeader_setWspecies(artHeader_t header, float value, int position)
{
	assert(header != NULL);
	assert(position >= 0 && position < header->nspecies);
	assert(isgreater(value, 0.0));

	header->wspecies[position] = value;
}

extern void
artHeader_setLspecies(artHeader_t header, int64_t value, int position)
{
	assert(header != NULL);
	assert(position >= 0 && position < header->nspecies);
	assert(value >= INT64_C(0));

	header->lspecies[position] = value;
}

extern void
artHeader_setBoxsizeInMpch(artHeader_t header, float boxsize)
{
	assert(header != NULL);
	assert(isgreater(boxsize, 0.0));

	header->extras[ARTHEADER_EXTRA_POS_BOXSIZE] = boxsize;

	local_calcFactorWeight(header);
	local_calcFactorPosition(header);
	local_calFactorVelocity(header);
}

extern void
artHeader_setFileEndianess(artHeader_t header, endian_t fileEndianess)
{
	assert(header != NULL);
	assert(fileEndianess == ENDIAN_LITTLE || fileEndianess == ENDIAN_BIG);

	header->fileEndianess = fileEndianess;
}

extern const char *
artHeader_getHeaderString(const artHeader_t header)
{
	assert(header != NULL);

	return header->headerString;
}

extern float
artHeader_getAexpn(const artHeader_t header)
{
	assert(header != NULL);

	return header->aexpn;
}

extern float
artHeader_getAexp0(const artHeader_t header)
{
	assert(header != NULL);

	return header->aexp0;
}

extern float
artHeader_getAmplt(const artHeader_t header)
{
	assert(header != NULL);

	return header->amplt;
}

extern float
artHeader_getAstep(const artHeader_t header)
{
	assert(header != NULL);

	return header->astep;
}

extern int
artHeader_getIstep(const artHeader_t header)
{
	assert(header != NULL);

	return header->istep;
}

extern float
artHeader_getPartw(const artHeader_t header)
{
	assert(header != NULL);

	return header->partw;
}

extern float
artHeader_getTintg(const artHeader_t header)
{
	assert(header != NULL);

	return header->tintg;
}

extern float
artHeader_getEkin(const artHeader_t header)
{
	assert(header != NULL);

	return header->ekin;
}

extern float
artHeader_getEkin1(const artHeader_t header)
{
	assert(header != NULL);

	return header->ekin1;
}

extern float
artHeader_getEkin2(const artHeader_t header)
{
	assert(header != NULL);

	return header->ekin2;
}

extern float
artHeader_getAu0(const artHeader_t header)
{
	assert(header != NULL);

	return header->au0;
}

extern float
artHeader_getAeu0(const artHeader_t header)
{
	assert(header != NULL);

	return header->aeu0;
}

extern int
artHeader_getNrowc(const artHeader_t header)
{
	assert(header != NULL);

	return header->nrowc;
}

extern int
artHeader_getNgridc(const artHeader_t header)
{
	assert(header != NULL);

	return header->ngridc;
}

extern int
artHeader_getNspecies(const artHeader_t header)
{
	assert(header != NULL);

	return header->nspecies;
}

extern int
artHeader_getNseed(const artHeader_t header)
{
	assert(header != NULL);

	return header->nseed;
}

extern float
artHeader_getOm0(const artHeader_t header)
{
	assert(header != NULL);

	return header->Om0;
}

extern float
artHeader_getOml0(const artHeader_t header)
{
	assert(header != NULL);

	return header->Oml0;
}

extern float
artHeader_getHubble(const artHeader_t header)
{
	assert(header != NULL);

	return header->hubble;
}

extern float
artHeader_getWp5(const artHeader_t header)
{
	assert(header != NULL);

	return header->wp5;
}

extern float
artHeader_getOcurv(const artHeader_t header)
{
	assert(header != NULL);

	return header->Ocurv;
}

extern float
artHeader_getExtra(const artHeader_t header, int position)
{
	assert(header != NULL);
	assert(position >= 0 && position < 100);

	return header->extras[position];
}

extern float
artHeader_getWspecies(const artHeader_t header, int position)
{
	assert(header != NULL);
	assert(position >= 0 && position < header->nspecies);

	return header->wspecies[position];
}

extern int64_t
artHeader_getLspecies(const artHeader_t header, int position)
{
	assert(header != NULL);
	assert(position >= 0 && position < header->nspecies);

	return header->lspecies[position];
}

extern double
artHeader_getBoxsizeInMpch(const artHeader_t header)
{
	assert(header != NULL);
	assert(isgreater(header->extras[ARTHEADER_EXTRA_POS_BOXSIZE], 0.0));

	return (double)(header->extras[ARTHEADER_EXTRA_POS_BOXSIZE]);
}

extern uint64_t
artHeader_getNumParticlesTotal(const artHeader_t header)
{
	assert(header != NULL);
	assert(header->nspecies > 0);

	uint64_t numParticlesTotal = UINT64_C(0);

	for (int i = 0; i < header->nspecies; i++)
		numParticlesTotal += header->lspecies[i];

	return numParticlesTotal;
}

extern double
artHeader_getFactorFileWeightToMsunh(const artHeader_t header)
{
	assert(header != NULL);

	return header->factorFileWeightToMsunh;
}

extern double
artHeader_getFactorFilePositionToMpch(const artHeader_t header)
{
	assert(header != NULL);

	return header->factorFilePositionToMpch;
}

extern double
artHeader_getFactorFileVelocityToKms(const artHeader_t header)
{
	assert(header != NULL);

	return header->factorFileVelocityToKms;
}

extern endian_t
artHeader_getFileEndianess(const artHeader_t header)
{
	assert(header != NULL);

	return header->fileEndianess;
}

extern void
artHeader_read(artHeader_t header, const char *headerFileName)
{
	FILE *f;

	assert(headerFileName != NULL);

	f                     = xfopen(headerFileName, "rb");
	header->fileEndianess = endian_getFileEndianessByBlockF(f);
	local_readHeaderActual(header, f);

	xfclose(&f);
}

extern void
artHeader_write(artHeader_t header, const char *headerFileName)
{
	assert(header != NULL);
	assert(headerFileName != NULL);

	FILE     *f;
	int      blocksize;
	endian_t systemEndianess = endian_getSystemEndianess();

	f         = xfopen(headerFileName, "wb");
	blocksize = ARTHEADER_HEADERSTRING_LENGTH + 121 * sizeof(float);
	if (header->fileEndianess != systemEndianess) {
		local_byteswapHeader(header);
		byteswap(&blocksize, sizeof(int));
	}

	xfwrite(&blocksize, sizeof(int), 1, f);
	xfwrite(header->headerString, sizeof(char),
	        ARTHEADER_HEADERSTRING_LENGTH, f);
	xfwrite(&(header->aexpn), sizeof(float), 121, f);
	xfwrite(&blocksize, sizeof(int), 1, f);

	if (header->fileEndianess != systemEndianess)
		local_byteswapHeader(header);

	xfclose(&f);
}

extern void
artHeader_prettyPrint(const artHeader_t header,
                      const char        *prefix,
                      FILE              *f)
{
	const char *actualPrefix = (prefix == NULL) ? "" : prefix;

	fprintf(f, "%s\n%sHeader values:\n", actualPrefix, actualPrefix);
	fprintf(f, "%s  headerString: %s\n", actualPrefix, header->headerString);
	fprintf(f, "%s  aexpn       : %f\n", actualPrefix, header->aexpn);
	fprintf(f, "%s  aexp0       : %f\n", actualPrefix, header->aexp0);
	fprintf(f, "%s  amplt       : %f\n", actualPrefix, header->amplt);
	fprintf(f, "%s  astep       : %f\n", actualPrefix, header->astep);
	fprintf(f, "%s  istep       : %i\n", actualPrefix, header->istep);
	fprintf(f, "%s  partw       : %f\n", actualPrefix, header->partw);
	fprintf(f, "%s  tintg       : %f\n", actualPrefix, header->tintg);
	fprintf(f, "%s  ekin        : %f\n", actualPrefix, header->ekin);
	fprintf(f, "%s  ekin1       : %f\n", actualPrefix, header->ekin1);
	fprintf(f, "%s  ekin2       : %f\n", actualPrefix, header->ekin2);
	fprintf(f, "%s  au0         : %f\n", actualPrefix, header->au0);
	fprintf(f, "%s  aeu0        : %f\n", actualPrefix, header->aeu0);
	fprintf(f, "%s  nrowc       : %i\n", actualPrefix, header->nrowc);
	fprintf(f, "%s  ngridc      : %i\n", actualPrefix, header->ngridc);
	fprintf(f, "%s  nspecies    : %i\n", actualPrefix, header->nspecies);
	fprintf(f, "%s  nseed       : %i\n", actualPrefix, header->nseed);
	fprintf(f, "%s  Om0         : %f\n", actualPrefix, header->Om0);
	fprintf(f, "%s  Oml0        : %f\n", actualPrefix, header->Oml0);
	fprintf(f, "%s  hubble      : %f\n", actualPrefix, header->hubble);
	fprintf(f, "%s  wp5         : %f\n", actualPrefix, header->wp5);
	fprintf(f, "%s  Ocurv       : %f\n", actualPrefix, header->Ocurv);

	for (int i = 0; i < header->nspecies; i++)
		fprintf(f, "%s  wspecies[%1i] : %f  (%g M_sun/h)\n",
		        actualPrefix, i, header->wspecies[i],
		        header->wspecies[i] * header->factorFileWeightToMsunh);
	for (int i = 0; i < header->nspecies; i++)
		fprintf(f, "%s  lspecies[%1i] : %" PRIi64 "\n",
		        actualPrefix, i, header->lspecies[i]);
	fprintf(f, "%s  boxsize     : %f  (extras[%2i])\n", actualPrefix,
	        header->extras[ARTHEADER_EXTRA_POS_BOXSIZE],
	        ARTHEADER_EXTRA_POS_BOXSIZE);

	fprintf(f, "%s  extras[%2i]  : %f\n",
	        actualPrefix, 0, header->extras[0]);
	for (int i = 1; i < 100; i++) {
		if (islessgreater(header->extras[i], header->extras[i - 1]))
			fprintf(f, "%s  extras[%2i]  : %f\n",
			        actualPrefix, i, header->extras[i]);
	}

	fprintf(f, "%s\n%sConversion factors\n", actualPrefix, actualPrefix);
	fprintf(f, "%s  weights to M_sun/h:  %g\n", actualPrefix,
	        header->factorFileWeightToMsunh);
	fprintf(f, "%s  position to Mpc/h :  %g\n", actualPrefix,
	        header->factorFilePositionToMpch);
	fprintf(f, "%s  velocity to km/s  :  %g\n", actualPrefix,
	        header->factorFileVelocityToKms);

	fprintf(f, "%s\n%sExtra Informations\n", actualPrefix, actualPrefix);
	fprintf(f, "%s  file endianess  :  %s\n", actualPrefix,
	        endian_toString(header->fileEndianess));
} /* artHeader_prettyPrint */

/*--- Implementation of local functions ---------------------------------*/
static void
local_nullHeader(artHeader_t header)
{
	for (int i = 0; i < ARTHEADER_HEADERSTRING_LENGTH + 1; i++)
		header->headerString[i] = '\0';
	header->aexpn    = HUGE_VAL;
	header->aexp0    = HUGE_VAL;
	header->amplt    = HUGE_VAL;
	header->astep    = HUGE_VAL;
	header->istep    = INT_MAX;
	header->partw    = HUGE_VAL;
	header->tintg    = HUGE_VAL;
	header->ekin     = HUGE_VAL;
	header->ekin1    = HUGE_VAL;
	header->ekin2    = HUGE_VAL;
	header->au0      = HUGE_VAL;
	header->aeu0     = HUGE_VAL;
	header->nrowc    = INT_MAX;
	header->ngridc   = INT_MAX;
	header->nspecies = INT_MAX;
	header->nseed    = INT_MAX;
	header->Om0      = HUGE_VAL;
	header->Oml0     = HUGE_VAL;
	header->hubble   = HUGE_VAL;
	header->wp5      = HUGE_VAL;
	header->Ocurv    = HUGE_VAL;
	for (int i = 0; i < 10; i++)
		header->wspecies[i] = HUGE_VAL;
	for (int i = 0; i < 10; i++)
		header->lspecies[i] = INT64_MAX;
	for (int i = 30; i < 100; i++)
		header->extras[i] = HUGE_VAL;

	header->fileEndianess            = endian_getSystemEndianess();
	header->factorFileWeightToMsunh  = 0.0;
	header->factorFilePositionToMpch = 0.0;
} /* local_nullHeader */

static void
local_readHeaderActual(artHeader_t header, FILE *f)
{
	uint32_t b1, b2;
	endian_t systemEndianess = endian_getSystemEndianess();

	xfread(&b1, sizeof(uint32_t), 1, f);
	xfread(header->headerString, sizeof(char),
	       ARTHEADER_HEADERSTRING_LENGTH, f);
	header->headerString[ARTHEADER_HEADERSTRING_LENGTH] = '\0';
	xfread(&(header->aexpn), sizeof(float), 121, f);
	xfread(&b2, sizeof(uint32_t), 1, f);

	if (b1 != b2) {
		diediedie(EXIT_FAILURE);
	}

	if (systemEndianess != header->fileEndianess)
		local_byteswapHeader(header);

	local_calcFactorWeight(header);
	local_calcFactorPosition(header);
	local_calFactorVelocity(header);
}

static void
local_byteswapHeader(artHeader_t header)
{
	for (int i = 0; i < 121; i++)
		byteswap(&(header->aexpn) + i, sizeof(float));
}

static void
local_calcFactorWeight(artHeader_t header)
{
	double boxsize      = artHeader_getBoxsizeInMpch(header);
	double numParticles = POW3(header->ngridc);
	double totalMass    = RHO_CRIT_0 * header->Om0 * POW3(boxsize);

	header->factorFileWeightToMsunh = totalMass / numParticles;
}

static void
local_calcFactorPosition(artHeader_t header)
{
	double boxsize = artHeader_getBoxsizeInMpch(header);

	header->factorFilePositionToMpch = boxsize / header->ngridc;
}

static void
local_calFactorVelocity(artHeader_t header)
{
	double boxsize = artHeader_getBoxsizeInMpch(header);

	header->factorFileVelocityToKms = 100. * boxsize
	                                  / (header->ngridc * header->aexpn);
}
