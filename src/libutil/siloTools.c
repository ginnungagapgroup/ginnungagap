// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "siloTools.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include <string.h>
#include <assert.h>


/*--- Local defines -----------------------------------------------------*/
#define SUFFIX    ".silo"
#define NUMDIGITS 5


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern char *
siloTools_getFileName(const char *siloBaseName, int rank, int size)
{
	char *name;
	int fileNumber;

	name = xmalloc(sizeof(char) * (strlen(siloBaseName)
	                               + strlen(SUFFIX)
	                               + NUMDIGITS + 1 + 1));
	fileNumber = rank < size ? rank : size-1;
	sprintf(name, "%s%s.%0*i", siloBaseName, SUFFIX, NUMDIGITS, fileNumber);

	return name;
}

extern char *
siloTools_getMasterFileName(const char *siloBaseName)
{
	char *name;

	name = xmalloc(sizeof(char) * (strlen(siloBaseName)
	                               + strlen(SUFFIX) + 1));
	sprintf(name, "%s%s", siloBaseName, SUFFIX);

	return name;
}

extern char *
siloTools_getGridName(const char *gridName, int rank, int size)
{
	char *name;

	name = xmalloc(sizeof(char) * (strlen(gridName)));

	sprintf(name, "%s", gridName);

	return name;
}

extern char *
siloTools_prependFileName(char *gridName, const char *siloFileName)
{
	char *result;

	result = xmalloc(sizeof(char) * (strlen(gridName)
	                                 + strlen(siloFileName)
	                                 + 1 + 1));
	sprintf(result, "%s:%s", siloFileName, gridName);
	xfree(gridName);

	return result;
}

/*--- Implementations of local functions --------------------------------*/
