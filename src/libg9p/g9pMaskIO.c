// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libg9p/g9pMaskIO.c
 * @ingroup  libg9pMaskIO
 * @brief  Implements the IO functionality for the mask.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMaskIO.h"
#include "g9pHierarchyIO.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "../libgrid/gridWriter.h"
#include "../libgrid/gridReader.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/

static g9pHierarchy_t
local_getHierachyFromIni(parse_ini_t ini, const char *sectionName);

static uint8_t
local_getMaskLevelFromIni(parse_ini_t ini, const char *sectionName);

static uint8_t
local_getMinLevelFromIni(parse_ini_t ini, const char *sectionName);

static uint8_t
local_getMaxLevelFromIni(parse_ini_t ini, const char *sectionName);

static uint8_t
local_getTileLevelFromIni(parse_ini_t ini, const char *sectionName);

static void
local_verifyLevels(uint8_t              maskLevel,
                   uint8_t              minLevel,
                   uint8_t              maxLevel,
                   uint8_t              tileLevel,
                   const g9pHierarchy_t h);

static void
local_mvDataMask2Grid(g9pMask_t mask, gridRegular_t grid);

static void
local_mvDataGrid2Mask(g9pMask_t mask, gridRegular_t grid);


/*--- Implementations of exported functions -----------------------------*/
extern g9pMask_t
g9pMaskIO_newFromIni(parse_ini_t    ini,
                     const char     *sectionName,
                     g9pHierarchy_t h)
{
	assert(ini != NULL);
	assert(sectionName != NULL);

	if (h == NULL)
		h = local_getHierachyFromIni(ini, sectionName);

	uint8_t maskLevel = local_getMaskLevelFromIni(ini, sectionName);
	uint8_t minLevel  = local_getMinLevelFromIni(ini, sectionName);
	uint8_t maxLevel  = local_getMaxLevelFromIni(ini, sectionName);
	uint8_t tileLevel = local_getTileLevelFromIni(ini, sectionName);

	local_verifyLevels(maskLevel, minLevel, maxLevel, tileLevel, h);

	return g9pMask_newMinMaxTiledMask(h, maskLevel, minLevel, maxLevel,
	                                  tileLevel);
}

extern void
g9pMaskIO_write(g9pMask_t mask, gridWriter_t writer)
{
	gridRegular_t grid = g9pMask_getEmptyGridStructure(mask);

	local_mvDataMask2Grid(mask, grid);

	gridWriter_activate(writer);
	gridWriter_writeGridRegular(writer, grid);
	gridWriter_deactivate(writer);

	local_mvDataGrid2Mask(mask, grid);

	gridRegular_del(&grid);
}

extern void
g9pMaskIO_read(g9pMask_t mask, gridReader_t reader)
{
	gridRegular_t grid = g9pMask_getEmptyGridStructure(mask);

	for (int i = 0; i < gridRegular_getNumPatches(grid); i++) {
		gridPatch_t patch = gridRegular_getPatchHandle(grid, i);
		gridReader_readIntoPatchForVar(reader, patch, 0);
	}

	local_mvDataGrid2Mask(mask, grid);

	gridRegular_del(&grid);
}

/*--- Implementations of local functions --------------------------------*/
static g9pHierarchy_t
local_getHierachyFromIni(parse_ini_t ini, const char *sectionName)
{
	g9pHierarchy_t h;
	char           *hSecName;

	getFromIni(&hSecName, parse_ini_get_string, ini, "hierarchySection",
	           sectionName);

	h = g9pHierarchyIO_newFromIni(ini, hSecName);

	xfree(hSecName);

	return h;
}

static uint8_t
local_getMaskLevelFromIni(parse_ini_t ini, const char *sectionName)
{
	int32_t maskLevel;

	getFromIni(&maskLevel, parse_ini_get_int32, ini, "maskLevel",
	           sectionName);

	if ((maskLevel < 0) || (maskLevel > G9PHIERARCHY_MAX_NUM_LEVELS)) {
		fprintf(stderr,
		        "FATAL: maskLevel is %i, but must be between 0 and %i.\n",
		        maskLevel, G9PHIERARCHY_MAX_NUM_LEVELS);
		exit(EXIT_FAILURE);
	}

	return (uint8_t)maskLevel;
}

static uint8_t
local_getMinLevelFromIni(parse_ini_t ini, const char *sectionName)
{
	int32_t minLevel;

	getFromIni(&minLevel, parse_ini_get_int32, ini, "minLevel",
	           sectionName);

	if ((minLevel < 0) || (minLevel > G9PHIERARCHY_MAX_NUM_LEVELS)) {
		fprintf(stderr,
		        "FATAL: minLevel is %i, but must be between 0 and %i.\n",
		        minLevel, G9PHIERARCHY_MAX_NUM_LEVELS);
		exit(EXIT_FAILURE);
	}

	return (uint8_t)minLevel;
}

static uint8_t
local_getMaxLevelFromIni(parse_ini_t ini, const char *sectionName)
{
	int32_t maxLevel;

	getFromIni(&maxLevel, parse_ini_get_int32, ini, "maxLevel",
	           sectionName);

	if ((maxLevel < 0) || (maxLevel > G9PHIERARCHY_MAX_NUM_LEVELS)) {
		fprintf(stderr,
		        "FATAL: maxLevel is %i, but must be between 0 and %i.\n",
		        maxLevel, G9PHIERARCHY_MAX_NUM_LEVELS);
		exit(EXIT_FAILURE);
	}

	return (uint8_t)maxLevel;
}

static uint8_t
local_getTileLevelFromIni(parse_ini_t ini, const char *sectionName)
{
	int32_t tileLevel;

	getFromIni(&tileLevel, parse_ini_get_int32, ini, "tileLevel",
	           sectionName);

	if ((tileLevel < 0) || (tileLevel > G9PHIERARCHY_MAX_NUM_LEVELS)) {
		fprintf(stderr,
		        "FATAL: maskLevel is %i, but must be between 0 and %i.\n",
		        tileLevel, G9PHIERARCHY_MAX_NUM_LEVELS);
		exit(EXIT_FAILURE);
	}

	return (uint8_t)tileLevel;
}

static void
local_verifyLevels(uint8_t              maskLevel,
                   uint8_t              minLevel,
                   uint8_t              maxLevel,
                   uint8_t              tileLevel,
                   const g9pHierarchy_t h)
{
	uint8_t hLevels = g9pHierarchy_getNumLevels(h);

	if (maxLevel > hLevels) {
		fprintf(stderr, "FATAL:  maxLevel (%i) > hierarchyLevels (%i)\n",
		        (int)maxLevel, (int)hLevels);
		exit(EXIT_FAILURE);
	}
	if (minLevel > maskLevel) {
		fprintf(stderr, "FATAL:  minLevel (%i) > maskLevel (%i)\n",
		        (int)minLevel, (int)maskLevel);
		exit(EXIT_FAILURE);
	}
	if (maxLevel < maskLevel) {
		fprintf(stderr, "FATAL:  maxLevel (%i) < maskLevel (%i)\n",
		        (int)maxLevel, (int)maskLevel);
		exit(EXIT_FAILURE);
	}
	if (tileLevel > minLevel) {
		fprintf(stderr, "FATAL:  tileLevel (%i) > minLevel (%i)\n",
		        (int)tileLevel, (int)minLevel);
		exit(EXIT_FAILURE);
	}
}

static void
local_mvDataMask2Grid(g9pMask_t mask, gridRegular_t grid)
{
	const uint32_t numTiles = g9pMask_getTotalNumTiles(mask);
	for (uint32_t i = 0; i < numTiles; i++) {
		gridPatch_t patch = gridRegular_getPatchHandle(grid, i);

		gridPatch_replaceVarData(patch, 0, g9pMask_getTileData(mask, i));
	}
}

static void
local_mvDataGrid2Mask(g9pMask_t mask, gridRegular_t grid)
{
	const uint32_t numTiles = g9pMask_getTotalNumTiles(mask);
	for (uint32_t i = 0; i < numTiles; i++) {
		int8_t      *d;
		gridPatch_t patch = gridRegular_getPatchHandle(grid, i);

		d = g9pMask_setTileData(mask, i, gridPatch_popVarData(patch, 0));

		if (d != NULL)
			diediedie(EXIT_FAILURE);
	}
}
