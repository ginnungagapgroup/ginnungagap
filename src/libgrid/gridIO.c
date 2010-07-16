// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridIO.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "../libutil/xfile.h"
#include "../libutil/byteswap.h"
#include "../libutil/diediedie.h"


/*--- Local variables ---------------------------------------------------*/
static const char *local_typeBovStr     = "bov";
static const char *local_typeSiloStr    = "silo";
static const char *local_typeGraficStr  = "grafic";
static const char *local_typeUnknownStr = "unknown";


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridIO_type_t
gridIO_getTypeFromName(const char *name)
{
	gridIO_type_t rtn;

	assert(name != NULL);

	if (strcmp(name, local_typeBovStr) == 0)
		rtn = IO_TYPE_BOV;
	else if (strcmp(name, local_typeSiloStr) == 0)
		rtn = IO_TYPE_SILO;
	else if (strcmp(name, local_typeGraficStr) == 0)
		rtn = IO_TYPE_GRAFIC;
	else
		rtn = IO_TYPE_UNKNOWN;

	return rtn;
}

extern const char *
gridIO_getNameFromType(gridIO_type_t type)
{
	const char *rtn;

	if (type == IO_TYPE_BOV)
		rtn = local_typeBovStr;
	else if (type == IO_TYPE_SILO)
		rtn = local_typeSiloStr;
	else if (type == IO_TYPE_GRAFIC)
		rtn = local_typeGraficStr;
	else
		rtn = local_typeUnknownStr;

	return rtn;
}

extern bool
gridIO_detectSwappingByFortranBlock(const char *fname)
{
	FILE *f;
	bool isSwapped = false;
	int  b1, b2, b1Swapped;

	assert(fname != NULL);

	f = xfopen(fname, "rb");

	xfread(&b1, sizeof(int), 1, f);
	b1Swapped = b1;
	byteswap(&b1Swapped, sizeof(int));
	if (b1Swapped < b1) {
		xfseek(f, (long)b1Swapped, SEEK_CUR);
		xfread(&b2, sizeof(int), 1, f);
	}
	if (b1 == b2) {
		// comparing unswapped values here to not have to swap b2
		isSwapped = true;
	} else {
		xfseek(f, 4L + b1, SEEK_SET);
		xfread(&b2, sizeof(int), 1, f);
		if (b1 == b2) {
			isSwapped = false;
		} else {
			fprintf(stderr,
			        "Cannot detect swapping, neither swapped nor "
			        "unswapped finds the end of the block :(\n");
			diediedie(EXIT_FAILURE);
		}
	}

	xfclose(&f);

	return isSwapped ? true : false;
} /* gridIO_detectSwappingByFortranBlock */

/*--- Implementations of local functions --------------------------------*/
