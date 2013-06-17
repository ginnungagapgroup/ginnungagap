// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMaskCreator.c
 * @ingroup  libg9pMaskCreator
 * @brief  Implements the creation routines for mask data.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMaskCreator.h"
#include "g9pMaskShapelet.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_initPatchData(gridPatch_t patch, uint8_t minLevel);

static void
local_tagCellsInPatch(gridPatch_t             patch,
                      uint64_t                numCells,
                      const gridPointUint32_t *cells,
                      const g9pMaskShapelet_t sl,
                      gridPointUint32_t       dimsGrid);

static bool
local_isCellAffectingPatch(const gridPointInt64_t dist,
                           int8_t                 extent);


/**
 * @brief  Helper function that performs the heavy-lifting in creating the
 *         mask.
 *
 * @param[in,out]  *maskData
 *                    The mask data attached to the patch of the grid of the
 *                    calling function.  This data must be correctly
 *                    described by @c idxLo and @c dimsPatch.
 * @param[in]      hiResCellIdxG
 *                    The index of the cell the function should work on in
 *                    grid coordinates (*not* patch coordinate).
 * @param[in]      *shape
 *                    The shapelet.
 * @param[in]      shapeDim1D
 *                    The size of the shapelet, this must be consistent with
 *                    the value used to generate the shapelet.
 * @param[in]      idxLo
 *                    The starting index of the patch of the calling
 *                    function, this is required to be able to convert patch
 *                    coordinates to grid coordinates.
 * @param[in]      dimsPatch
 *                    The extent of the patch.
 * @param[in]      dimsGrid
 *                    The extent of the grid.
 *
 * @return  Returns nothing.
 */
inline static void
local_throwShapeOnMask(int8_t *restrict        maskData,
                       const gridPointUint32_t hiResCellIdxG,
                       const int8_t *restrict  shape,
                       uint32_t                shapeDim1D,
                       const gridPointUint32_t idxLo,
                       const gridPointUint32_t dimsPatch,
                       const gridPointUint32_t dimsGrid);

static void
local_fixTaintedLowLevelCells(gridPatch_t patch, const g9pMask_t mask);

static bool
isSubVolumeCorrect(const int8_t *d,
                   uint32_t     dimSub,
                   uint32_t     dim,
                   int8_t       expected);

static void
correctSubVolume(int8_t   *d,
                 uint32_t dimSub,
                 uint32_t dim,
                 int8_t   newVal);


/*--- Implementations of exported functions -----------------------------*/
extern void
g9pMaskCreator_fromCells(g9pMask_t               mask,
                         uint64_t                numCells,
                         const gridPointUint32_t *cells)
{
	assert(mask != NULL);
	assert((numCells > 0 && cells != NULL) || (numCells == 0));

	gridRegular_t     grid = g9pMask_getEmptyGridStructure(mask);
	g9pMaskShapelet_t sl   = g9pMaskShapelet_new(g9pMask_getMinLevel(mask),
	                                             g9pMask_getMaxLevel(mask));

	gridPointUint32_t gridDims;
	gridRegular_getDims(grid, gridDims);

	const uint32_t totalNumTiles = g9pMask_getTotalNumTiles(mask);
#ifdef WITH_OPENMP
#  pragma omp parallel for
#endif
	for (uint32_t i = 0; i < totalNumTiles; i++) {
		gridPatch_t patch = gridRegular_getPatchHandle(grid, i);
		local_initPatchData(patch, g9pMask_getMinLevel(mask));
		local_tagCellsInPatch(patch, numCells, cells, sl, gridDims);
		local_fixTaintedLowLevelCells(patch, mask);
		g9pMask_setTileData(mask, i, gridPatch_popVarData(patch, 0));
	}
	g9pMaskShapelet_del(&sl);
	gridRegular_del(&grid);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_initPatchData(gridPatch_t patch, uint8_t minLevel)
{
	int8_t   *data    = gridPatch_getVarDataHandle(patch, 0);
	uint64_t numCells = gridPatch_getNumCells(patch);

	for (uint64_t i = 0; i < numCells; i++)
		data[i] = minLevel;
}

static void
local_tagCellsInPatch(gridPatch_t             patch,
                      uint64_t                numCells,
                      const gridPointUint32_t *cells,
                      const g9pMaskShapelet_t sl,
                      gridPointUint32_t       dimsGrid)
{
	gridPointUint32_t idxLo, dims;
	int8_t            *data = gridPatch_getVarDataHandle(patch, 0);

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);

	int8_t       extent  = g9pMaskShapelet_getExtent(sl);
	int8_t       slDim1D = g9pMaskShapelet_getDim1D(sl);
	const int8_t *slData = g9pMaskShapelet_getData(sl);

	for (uint64_t i = 0; i < numCells; i++) {
		gridPointInt64_t dist;
		(void)gridPatch_calcDistanceVector(patch, cells[i], dimsGrid, dist);
		if (!local_isCellAffectingPatch(dist, extent))
			continue;
		local_throwShapeOnMask(data, cells[i], slData, slDim1D,
		                       idxLo, dims, dimsGrid);
	}
}

static bool
local_isCellAffectingPatch(const gridPointInt64_t dist,
                           int8_t                 extent)
{
	bool isAffecting = false;

	for (int i = 0; i < NDIM; i++) {
		if (((dist[i] < 0) && (-dist[i] < extent)) || (dist[i] < extent))
			isAffecting = true;
	}

	return isAffecting;
}

inline static void
local_throwShapeOnMask(int8_t *restrict        maskData,
                       const gridPointUint32_t hiResCellIdxG,
                       const int8_t *restrict  shape,
                       uint32_t                shapeDim1D,
                       const gridPointUint32_t idxLo,
                       const gridPointUint32_t dimsPatch,
                       const gridPointUint32_t dimsGrid)
{
	uint32_t iSG, jSG, kSG; // position of shape (S) in grid coordinates (G)
	int32_t  iSM, jSM, kSM; // position of shape (S) in map coordinates (M)
	int32_t  shapeExtent = shapeDim1D / 2;
	uint64_t idxS, idxM;

	assert(shapeDim1D % 2 == 1);

	for (int32_t k = -shapeExtent; k <= shapeExtent; k++) {
		kSG = (hiResCellIdxG[2] + k + dimsGrid[2]) % dimsGrid[2];
		kSM = kSG - idxLo[2];
		if ((kSM < 0) || (kSM >= dimsPatch[2]))
			continue;
		for (int32_t j = -shapeExtent; j <= shapeExtent; j++) {
			jSG = (hiResCellIdxG[1] + j + dimsGrid[1]) % dimsGrid[1];
			jSM = jSG - idxLo[1];
			if ((jSM < 0) || (jSM >= dimsPatch[1]))
				continue;
			for (int32_t i = -shapeExtent; i <= shapeExtent; i++) {
				iSG = (hiResCellIdxG[0] + i + dimsGrid[0]) % dimsGrid[0];
				iSM = iSG - idxLo[0];
				if ((iSM < 0) || (iSM >= dimsPatch[0]))
					continue;
				idxS = (i + shapeExtent)
				       + (j + shapeExtent) * shapeDim1D
				       + (k + shapeExtent) * shapeDim1D * shapeDim1D;
				idxM = iSM + (jSM + kSM * dimsPatch[1]) * dimsPatch[0];
				if (maskData[idxM] < shape[idxS]) {
					maskData[idxM] = shape[idxS];
				}
			}
		}
	}
} /* local_throwShapeOnMask */

static void
local_fixTaintedLowLevelCells(gridPatch_t patch, const g9pMask_t mask)
{
	g9pHierarchy_t h         = g9pMask_getHierarchyRef(mask);
	int8_t         *d        = gridPatch_getVarDataHandle(patch, 0);

	int8_t         curLevel  = g9pMask_getMinLevel(mask);
	int8_t         maskLevel = g9pMask_getMaskLevel(mask);
	int8_t         tileLevel = g9pMask_getTileLevel(mask);
	uint32_t       dimMask   = g9pHierarchy_getFactorBetweenLevel(h,
	                                                              tileLevel,
	                                                              maskLevel);

	while (curLevel < maskLevel) {
		uint32_t dimLevel = g9pHierarchy_getFactorBetweenLevel(h, curLevel,
		                                                       tileLevel);
		uint32_t factor   = g9pHierarchy_getFactorBetweenLevel(h, curLevel,
		                                                       maskLevel);
#if (NDIM > 2)
		for (uint32_t z = 0; z < dimLevel; z++)
#else
		const uint32_t z = 0;
#endif
		{
			for (uint32_t y = 0; y < dimLevel; y++) {
				for (uint32_t x = 0; x < dimLevel; x++) {
					uint64_t idx = (x + (y + z * dimMask) * dimMask)
					               * factor;
					if (!isSubVolumeCorrect(d + idx, factor, dimMask,
					                        curLevel)) {
						correctSubVolume(d + idx, factor, dimMask,
						                 curLevel + 1);
					}
				}
			}
		}

		curLevel++;
	}

	g9pHierarchy_del(&h);
} /* local_fixTaintedLowLevelCells */

static bool
isSubVolumeCorrect(const int8_t *d,
                   uint32_t     dimSub,
                   uint32_t     dim,
                   int8_t       expected)
{
#if (NDIM > 2)
	for (uint32_t z = 0; z < dimSub; z++)
#else
	const uint32_t z = 0;
#endif
	{
		for (uint32_t y = 0; y < dimSub; y++) {
			for (uint32_t x = 0; x < dimSub; x++) {
				uint64_t idx = x + (y + z * dim) * dim;
				if (d[idx] != expected)
					return false;
			}
		}
	}

	return true;
}

static void
correctSubVolume(int8_t   *d,
                 uint32_t dimSub,
                 uint32_t dim,
                 int8_t   newVal)
{
#if (NDIM > 2)
	for (uint32_t z = 0; z < dimSub; z++)
#else
	const uint32_t z = 0;
#endif
	{
		for (uint32_t y = 0; y < dimSub; y++) {
			for (uint32_t x = 0; x < dimSub; x++) {
				uint64_t idx = x + (y + z * dim) * dim;
				d[idx] = d[idx] < newVal ? newVal : d[idx];
			}
		}
	}
}
