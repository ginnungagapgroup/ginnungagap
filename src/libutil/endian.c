// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/endian.c
 * @ingroup libutilMiscEndian
 * @brief  This file implements the functionality to deal with endianess.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "endian.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include "xfile.h"
#include "diediedie.h"
#include "byteswap.h"


/*--- Static variables --------------------------------------------------*/

/** @brief  Holds the string version for little endian. */
static char endian_str_little[] = "little endian";

/** @brief  Holds the string version for big endian. */
static char endian_str_big[] = "big endian";


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Tries to find the end of the block.
 *
 * @param[in]      b1
 *                    The value of the leading integer giving the block
 *                    size as read from the file.
 * @param[in]      b1Swapped
 *                    This is @c b1 byte-swapped.
 * @param[in,out]  *f
 *                    The file to work with.
 *
 * @return  Returns the value of the trailing integer giving the block
 *          size.
 */
static uint32_t
local_findBlockEnd(uint32_t b1, uint32_t b1Swapped, FILE *f);


/**
 * @brief  Sorts out what the file endianess is.
 *
 * Essentially, this check whether the file has the same endianess than
 * the system, or not.
 *
 * @param[in]  b1
 *                The value of the first delimiter as read from the
 *                file.
 * @param[in]  b1Swapped
 *                The value of the first delimiter in the opposite
 *                endianess as in the file.
 * @param[in]  b2
 *                The second delimiter as read from the file.
 *
 * @return  Returns the endianess of the file.
 */
static endian_t
local_decideOnFileEndian(uint32_t b1, uint32_t b1Swapped, uint32_t b2);


/*--- Implementations of exported functios ------------------------------*/
extern endian_t
endian_getSystemEndianess(void)
{
	endian_t         systemEndianess;
	static const int temp = 1;
	unsigned char    *p   = (unsigned char *)&temp;

	// This verifies that the in-memory representation of 1 is
	// 00 00 00 01 (which is big endian)
	// and not
	// 01 00 00 00 (which would be little endian)

	// Otherwise the magic might not work as intended
	assert(4 * sizeof(unsigned char) == sizeof(int));

	if ((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] == 1))
		systemEndianess = ENDIAN_BIG;
	else if ((p[0] == 1) && (p[1] == 0) && (p[2] == 0) && (p[3] == 0))
		systemEndianess = ENDIAN_LITTLE;
	else
		diediedie(EXIT_FAILURE);

	return systemEndianess;
}

extern bool
endian_systemIsLittle(void)
{
	return (endian_getSystemEndianess() == ENDIAN_LITTLE) ? true : false;
}

extern bool
endian_systemIsBig(void)
{
	return (endian_getSystemEndianess() == ENDIAN_BIG) ? true : false;
}

extern endian_t
endian_getFileEndianessByBlock(const char *fname)
{
	FILE     *f;
	endian_t fileEndianess;

	assert(fname != NULL);

	f             = xfopen(fname, "rb");
	fileEndianess = endian_getFileEndianessByBlockF(f);
	xfclose(&f);

	return fileEndianess;
}

extern bool
endian_fileIsLittleByBlock(const char *fname)
{
	endian_t fileEndianess;

	assert(fname != NULL);

	fileEndianess = endian_getFileEndianessByBlock(fname);

	return (fileEndianess == ENDIAN_LITTLE) ? true : false;
}

extern bool
endian_fileIsBigByBlock(const char *fname)
{
	endian_t fileEndianess;

	assert(fname != NULL);

	fileEndianess = endian_getFileEndianessByBlock(fname);

	return (fileEndianess == ENDIAN_BIG) ? true : false;
}

extern endian_t
endian_getFileEndianessByBlockF(FILE *f)
{
	int      b1, b2, b1Swapped;
	endian_t fileEndianess;
	long     pos = ftell(f);

	assert(f != NULL);

	xfread(&b1, sizeof(int), 1, f);
	b1Swapped = b1;
	byteswap(&b1Swapped, sizeof(int));
	b2        = local_findBlockEnd(b1, b1Swapped, f);

	xfseek(f, pos, SEEK_SET);

	fileEndianess = local_decideOnFileEndian(b1, b1Swapped, b2);

	return fileEndianess;
}

extern bool
endian_fileIsLittleByBlockF(FILE *f)
{
	endian_t fileEndianess;

	assert(f != NULL);

	fileEndianess = endian_getFileEndianessByBlockF(f);

	return (fileEndianess == ENDIAN_LITTLE) ? true : false;
}

extern bool
endian_fileIsBigByBlockF(FILE *f)
{
	endian_t fileEndianess;

	assert(f != NULL);

	fileEndianess = endian_getFileEndianessByBlockF(f);

	return (fileEndianess == ENDIAN_BIG) ? true : false;
}

extern const char *
endian_toString(const endian_t endian)
{
	if (endian == ENDIAN_LITTLE)
		return endian_str_little;

	return endian_str_big;
}

/*--- Implementations of local functions --------------------------------*/
static uint32_t
local_findBlockEnd(uint32_t b1, uint32_t b1Swapped, FILE *f)
{
	uint32_t b2;
	long     pos = ftell(f);

	if (b1 < b1Swapped) {
		xfseek(f, (long)b1, SEEK_CUR);
		xfread(&b2, sizeof(int), 1, f);
		if (b1 != b2) {
			xfseek(f, pos + (long)b1Swapped, SEEK_SET);
			xfread(&b2, sizeof(uint32_t), 1, f);
			byteswap(&b2, sizeof(uint32_t));
		}
	} else {
		xfseek(f, (long)b1Swapped, SEEK_CUR);
		xfread(&b2, sizeof(uint32_t), 1, f);
		byteswap(&b2, sizeof(uint32_t));
		if (b1Swapped != b2) {
			xfseek(f, pos + (long)b1, SEEK_SET);
			xfread(&b2, sizeof(uint32_t), 1, f);
		}
	}

	xfseek(f, pos, SEEK_SET);

	return b2;
}

static endian_t
local_decideOnFileEndian(uint32_t b1, uint32_t b1Swapped, uint32_t b2)
{
	endian_t fileEndianess;
	endian_t systemEndianess = endian_getSystemEndianess();

	if (b1 == b2)
		fileEndianess = systemEndianess;
	else if (b1Swapped == b2)
		fileEndianess = (systemEndianess == ENDIAN_BIG)
		                ? ENDIAN_LITTLE : ENDIAN_BIG;
	else
		diediedie(EXIT_FAILURE);

	return fileEndianess;
}
