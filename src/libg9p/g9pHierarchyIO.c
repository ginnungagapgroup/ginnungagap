// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchyIO.c
 * @ingroup  libg9pHierarchyIO
 * @brief  Implements the IO routines for @ref libg9pHierarchy.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchyIO.h"
#include <assert.h>
#include <stdlib.h>
#include "../libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static uint8_t
local_getNumLevelsFromIni(parse_ini_t ini, const char *sectionName);

static g9pHierarchy_t
local_getFactorHierarchy(parse_ini_t ini,
                         const char  *sectionName,
                         uint8_t     numLevels,
                         uint32_t    minDim1D);

static g9pHierarchy_t
local_getDimHierarchy(parse_ini_t ini,
                      const char  *sectionName,
                      uint8_t     numLevels);


/*--- Implementations of exported functions -----------------------------*/
extern g9pHierarchy_t
g9pHierarchyIO_newFromIni(parse_ini_t ini, const char *sectionName)
{
	assert(ini != NULL);
	assert(sectionName != NULL);

	uint8_t  numLevels = local_getNumLevelsFromIni(ini, sectionName);

	uint32_t minDim1D;
	if (parse_ini_get_uint32(ini, "minDim1D", sectionName, &minDim1D)) {
		return local_getFactorHierarchy(ini, sectionName, numLevels,
		                                minDim1D);
	} else {
		return local_getDimHierarchy(ini, sectionName, numLevels);
	}
}

/*--- Implementations of local functions --------------------------------*/
static uint8_t
local_getNumLevelsFromIni(parse_ini_t ini, const char *sectionName)
{
	int32_t numLevels;

	getFromIni(&numLevels, parse_ini_get_int32, ini, "numLevels",
	           sectionName);

	if ((numLevels < 0) || (numLevels > G9PHIERARCHY_MAX_NUM_LEVELS)) {
		fprintf(stderr,
		        "FATAL: numLevels is %i, but must be between 0 and %i.\n",
		        numLevels, G9PHIERARCHY_MAX_NUM_LEVELS);
		exit(EXIT_FAILURE);
	}

	return (uint8_t)numLevels;
}

static g9pHierarchy_t
local_getFactorHierarchy(parse_ini_t ini,
                         const char  *sectionName,
                         uint8_t     numLevels,
                         uint32_t    minDim1D)
{
	uint32_t factor;

	if (parse_ini_get_uint32(ini, "factor", sectionName, &factor)) {
		return g9pHierarchy_newWithSimpleFactor(numLevels, minDim1D, factor);
	} else {
		g9pHierarchy_t h;
		uint32_t       *factors;

		if (!parse_ini_get_int32list(ini, "factors", sectionName,
		                             numLevels - 1,
		                             (int32_t **)&factors)) {
			fprintf(stderr,
			        "FATAL: Must set either factor, or factors with "
			        "exactly %i elements.\n",
			        (int)numLevels - 1);
			exit(EXIT_FAILURE);
		}

		h = g9pHierarchy_newWithExplicitFactors(numLevels,
		                                        minDim1D,
		                                        factors);
		xfree(factors);

		return h;
	}
}

static g9pHierarchy_t
local_getDimHierarchy(parse_ini_t ini,
                      const char  *sectionName,
                      uint8_t     numLevels)
{
	uint32_t       *dims;
	g9pHierarchy_t h;

	if (!parse_ini_get_int32list(ini, "dim1Ds", sectionName, numLevels,
	                             (int32_t **)&dims)) {
		fprintf(stderr,
		        "FATAL: Must set either minDim1D, or dim1Ds with "
		        "exactly %i elements.\n",
		        (int)numLevels);
		exit(EXIT_FAILURE);
	}

	h = g9pHierarchy_newWithDims(numLevels, dims);
	xfree(dims);

	return h;
}
