// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "grid_config.h"
#include "gridSiloTools.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include <assert.h>


/*--- Local defines -----------------------------------------------------*/
#define GRIDSILOTOOLS_FILESUFFIX       ".silo"
#define GRIDSILOTOOLS_FILENUMBERDIGITS 5


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern char *
gridSiloTools_getSiloFileName(char       *siloFileName,
                              size_t     *n,
                              const char *siloBaseName,
                              int        rank)
{
	size_t lengthSiloFileName;
	assert(n != NULL && siloBaseName != NULL && rank >= 0);
	lengthSiloFileName = strlen(siloBaseName)
	                     + 1
	                     + GRIDSILOTOOLS_FILENUMBERDIGITS
	                     + strlen(GRIDSILOTOOLS_FILESUFFIX)
	                     + 1;
	if (*n < lengthSiloFileName) {
		siloFileName = xrealloc(siloFileName, lengthSiloFileName);
		*n           = lengthSiloFileName;
	}
	sprintf(siloFileName, "%s.%0*i%s",
	        siloBaseName, GRIDSILOTOOLS_FILENUMBERDIGITS, rank,
	        GRIDSILOTOOLS_FILESUFFIX);

	return siloFileName;
}

extern char *
gridSiloTools_getSiloMasterFileName(char       *siloFileName,
                                    size_t     *n,
                                    const char *siloBaseName)
{
	size_t lengthSiloFileName;
	assert(n != NULL && siloBaseName != NULL);
	lengthSiloFileName = strlen(siloBaseName)
	                     + strlen(GRIDSILOTOOLS_FILESUFFIX)
	                     + 1;
	if (*n < lengthSiloFileName) {
		siloFileName = xrealloc(siloFileName, lengthSiloFileName);
		*n           = lengthSiloFileName;
	}
	sprintf(siloFileName, "%s%s", siloBaseName, GRIDSILOTOOLS_FILESUFFIX);

	return siloFileName;
}

extern char *
gridSiloTools_getSiloMeshName(char       *siloMeshName,
                              size_t     *n,
                              const char *meshName,
                              int        rank)
{
	size_t lengthSiloMeshName;
	assert(n != NULL && meshName != NULL && rank >= 0);
	lengthSiloMeshName = strlen(meshName)
	                     + 1
	                     + GRIDSILOTOOLS_FILENUMBERDIGITS
	                     + 1;
	if (*n < lengthSiloMeshName) {
		siloFileName = xrealloc(siloFileName, lengthSiloMeshName);
		*n           = lengthSiloMeshName;
	}
	sprintf(siloMeshName, "%s.%0*i",
	        siloFileName, meshName, GRIDSILOTOOLS_FILENUMBERDIGITS, rank);
	return siloMeshName;
}

extern char *
gridSiloTools_getSiloVarName(char       *siloVarName,
                             size_t     *n,
                             const char *varName,
                             int        rank)
{
	return gridSiloTools_getSiloMeshName(siloVarName, n, varName, rank);
}

/*--- Implementations of local functions --------------------------------*/
