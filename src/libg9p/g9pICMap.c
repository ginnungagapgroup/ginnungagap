// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pICMap.c
 * @ingroup  GROUP
 * @brief  SHORT DESC
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pICMap.h"
#include <assert.h>
#include <string.h>
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "g9pICMap_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_calcIdx(g9pICMap_t map, uint32_t zoomlevel);

static void
local_calcNumCellsPerFile(g9pICMap_t map);


/*--- Implementations of exported functions -----------------------------*/
extern g9pICMap_t
g9pICMap_new(uint32_t     numFiles,
             uint32_t     numGasLevel,
             const int8_t *gasLevel,
             g9pMask_t    mask,
             uint32_t zoomlevel)
{
	g9pICMap_t map;

	assert(mask != NULL);
	assert(numFiles > 0);
	assert(numFiles <= g9pMask_getTotalNumTiles(mask));

	map              = xmalloc(sizeof(struct g9pICMap_struct));
	map->numFiles    = numFiles;
	map->mask        = mask;
	map->hierarchy   = g9pMask_getHierarchyRef(mask);
	assert(numGasLevel <= g9pHierarchy_getNumLevels(map->hierarchy));
	map->numGasLevel = numGasLevel;
	if (map->numGasLevel > 0) {
		map->gasLevel = xmalloc(sizeof(int8_t) * map->numGasLevel);
		memcpy(map->gasLevel, gasLevel, sizeof(int8_t) * map->numGasLevel);
	} else {
		map->gasLevel = NULL;
	}

	const int8_t numLevel = g9pMask_getNumLevel(map->mask);
	map->firstTileIdx = xmalloc(sizeof(uint32_t) * map->numFiles * 2);
	map->lastTileIdx  = map->firstTileIdx + map->numFiles;
	map->numCells     = xmalloc(sizeof(uint64_t)
	                            * (map->numFiles * numLevel));

	local_calcIdx(map,zoomlevel);
	local_calcNumCellsPerFile(map);

	return map;
}

extern void
g9pICMap_del(g9pICMap_t *g9pICMap)
{
	assert(g9pICMap != NULL && *g9pICMap != NULL);

	xfree((*g9pICMap)->firstTileIdx);
	xfree((*g9pICMap)->numCells);
	if ((*g9pICMap)->gasLevel != NULL)
		xfree((*g9pICMap)->gasLevel);
	g9pMask_del(&((*g9pICMap)->mask));
	g9pHierarchy_del(&((*g9pICMap)->hierarchy));
	xfree(*g9pICMap);

	*g9pICMap = NULL;
}

extern uint32_t
g9pICMap_getFileForTile(const g9pICMap_t map, const uint32_t tile)
{
	assert(map != NULL);
	assert(tile < g9pMask_getTotalNumTiles(map->mask));

	uint32_t file = 0;
	while ((file < map->numFiles) && (map->lastTileIdx[file] < tile)) {
		file++;
	}
	assert(file < map->numFiles);
	assert(map->lastTileIdx[file] >= tile);
	assert(map->firstTileIdx[file] <= tile);

	return file;
}

extern uint32_t
g9pICMap_getFirstTileInFile(const g9pICMap_t map, const uint32_t file)
{
	assert(map != NULL);
	assert(file < map->numFiles);

	return map->firstTileIdx[file];
}

extern uint32_t
g9pICMap_getLastTileInFile(const g9pICMap_t map, const uint32_t file)
{
	assert(map != NULL);
	assert(file < map->numFiles);

	return map->lastTileIdx[file];
}

extern const uint64_t *
g9pICMap_getNumCellsPerLevelInFile(const g9pICMap_t map,
                                   const uint32_t   file)
{
	assert(map != NULL);
	assert(file < map->numFiles);

	return map->numCells + (file * g9pMask_getNumLevel(map->mask));
}

/*--- Implementations of local functions --------------------------------*/
static void
local_calcIdx(g9pICMap_t map, uint32_t zoomlevel)
{
	uint32_t numTiles     = g9pMask_getTotalNumTiles(map->mask);
	//uint32_t numTilesLeft = numTiles;
	//uint32_t tilePerFile  = numTilesLeft / map->numFiles;
	uint64_t numCellsTot = 0;
	for (uint32_t i=0; i<numTiles; i++) {
		numCellsTot+=g9pMask_getNumCellsInTileForLevel(map->mask, i, zoomlevel);
	}
	uint64_t numCellsLeft = numCellsTot;
	uint32_t cellsPerFile = numCellsLeft / map->numFiles;

	map->firstTileIdx[0] = 0;
	//map->lastTileIdx[0]  = 0;
	//numTilesLeft        -= tilePerFile;
	uint32_t numCells = 0;
	uint32_t file = 0;
	for (uint32_t i = 0; i < numTiles; i++) {
		numCells += g9pMask_getNumCellsInTileForLevel(map->mask, i, zoomlevel);
		if (numCells >= cellsPerFile) {
			map->lastTileIdx[file] = i;
			numCellsLeft -= numCells;
			numCells = 0;
			file++;
			if(file < map->numFiles) 
				cellsPerFile = numCellsLeft / (map->numFiles - file);
			if(file < map->numFiles)
				map->firstTileIdx[file] = i+1;
		}
		
		//tilePerFile          = numTilesLeft / (map->numFiles - i);
		//map->firstTileIdx[i] = map->lastTileIdx[i - 1] + 1;
		//map->lastTileIdx[i]  = map->firstTileIdx[i] + tilePerFile - 1;
		//numTilesLeft        -= tilePerFile;
	}
	map->lastTileIdx[map->numFiles - 1] = numTiles - 1;
}

static void
local_calcNumCellsPerFile(g9pICMap_t map)
{
	const int8_t numLevel = g9pMask_getNumLevel(map->mask);
	uint64_t     *tmp     = NULL;

	for (uint32_t i = 0; i < map->numFiles; i++) {
		uint32_t j = map->firstTileIdx[i];
		for (int8_t k = 0; k < numLevel; k++)
			map->numCells[i * numLevel + k] = UINT64_C(0);
		do {
			tmp = g9pMask_getNumCellsInTile(map->mask, j, tmp);
			for (int8_t k = 0; k < numLevel; k++)
				map->numCells[i * numLevel + k] += tmp[k];
		} while (++j <= map->lastTileIdx[i]);
	}
	xfree(tmp);
}
