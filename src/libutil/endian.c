// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "endian.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "xfile.h"
#include "diediedie.h"
#include "byteswap.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static int
local_findBlockEnd(int b1, int b1Swapped, FILE *f);

static bool
local_decideOnFileEndian(int b1, int b1Swapped, int b2, bool systemIsBig);


/*--- Implementations of exported functios ------------------------------*/
extern bool
endian_systemIsLittle(void)
{
	return endian_systemIsBig() ? false : true;
}

extern bool
endian_systemIsBig(void)
{
	static const int temp = 1;
	unsigned char    *p   = (unsigned char *)&temp;

	// This verifies that the in-memory representation of 1 is
	// 00 00 00 01 (which is big endian)
	// and not
	// 01 00 00 00 (which would be little endian)

	// Otherwise the magic might not work as intended
	assert(4 * sizeof(unsigned char) == sizeof(int));

	return ((p[0] == 0) && (p[1] == 0) && (p[2] == 0) && (p[3] == 1))
	       ? true : false;
}

extern endian_t
endian_getSystemEndianess(void)
{
	return endian_systemIsBig() ? ENDIAN_BIG : ENDIAN_LITTLE;
}

extern bool
endian_fileIsLittleByBlock(const char *fname)
{
	return (endian_fileIsBigByBlock(fname)) ? false : true;
}

extern bool
endian_fileIsBigByBlock(const char *fname)
{
	FILE *f;
	bool systemIsBig = endian_systemIsBig();
	int  b1, b2, b1Swapped;

	assert(fname != NULL);

	f = xfopen(fname, "rb");
	xfread(&b1, sizeof(int), 1, f);
	b1Swapped = b1;
	byteswap(&b1Swapped, sizeof(int));
	b2 = local_findBlockEnd(b1, b1Swapped, f);
	xfclose(&f);

	return local_decideOnFileEndian(b1, b1Swapped, b2, systemIsBig);
}

extern endian_t
endian_getFileEndianessByBlock(const char *fname)
{
	assert(fname != NULL);

	return endian_fileIsBigByBlock(fname) ? ENDIAN_BIG : ENDIAN_LITTLE;
}

/*--- Implementations of local functions --------------------------------*/
static int
local_findBlockEnd(int b1, int b1Swapped, FILE *f)
{
	int  b2;
	long pos = ftell(f);

	if (b1 < b1Swapped) {
		xfseek(f, (long)b1, SEEK_CUR);
		xfread(&b2, sizeof(int), 1, f);
		if (b1 != b2) {
			xfseek(f, pos + (long)b1Swapped, SEEK_SET);
			xfread(&b2, sizeof(int), 1, f);
			byteswap(&b2, sizeof(int));
		}
	} else {
		xfseek(f, (long)b1Swapped, SEEK_CUR);
		xfread(&b2, sizeof(int), 1, f);
		byteswap(&b2, sizeof(int));
		if (b1Swapped != b2) {
			xfseek(f, pos + (long)b1, SEEK_SET);
			xfread(&b2, sizeof(int), 1, f);
		}
	}

	return b2;
}

static bool
local_decideOnFileEndian(int b1, int b1Swapped, int b2, bool systemIsBig)
{
	bool fileIsBig;

	if (b1 == b2)
		fileIsBig = systemIsBig;
	else if (b1Swapped == b2)
		fileIsBig = !systemIsBig;
	else
		diediedie(EXIT_FAILURE);

	return fileIsBig;
}
