// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "makeMask.h"
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_OPENMP
#  include <omp.h>
#endif
#include "../../src/libdata/dataVar.h"
#include "../../src/libdata/dataVarType.h"
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"
#include "../../src/libgrid/gridWriterFactory.h"
#include "../../src/libgrid/gridPatch.h"
#include "../../src/libgrid/gridHistogram.h"
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/timer.h"


/*--- Implemention of main structure ------------------------------------*/
#include "makeMask_adt.h"


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Helper function to generate the grid.
 *
 * @param[in]  setup
 *                The setup object holding the required information.
 *
 * @return  Returns a new grid that will hold the mask.
 */
inline static gridRegular_t
local_getGrid(const makeMaskSetup_t setup);


/**
 * @brief  Helper function to generate the grid distribution.
 *
 * @param[in]  mama
 *                The makeMask object that holds the required information.
 *
 * @return  Returns a new distribution object.
 */
inline static gridRegularDistrib_t
local_getDistrib(const makeMask_t mama);


/**
 * @brief  Helper function to initialize the mask.
 *
 * This will create the mask variable, allocate the required patch, and fill
 * the patch with the base refinement level.
 *
 * @param[in,out]  mama
 *                    The makeMask object which with to work.
 *
 * @return  Returns nothing.
 */
inline static void
local_createEmptyMask(makeMask_t mama);


/**
 * @brief  Helper function to tag the level at which each cell of the mask
 *         should be generated.
 *
 * @param[in,out]  mama
 *                    The makeMask object to work with.
 *
 * @return  Returns nothing.
 */
inline static void
local_markRegions(makeMask_t mama);


/**
 * @brief  Helper function to write the mask to disk.
 *
 * @param[in]  mama
 *                The makeMask object to work with.
 *
 * @return  Returns nothing.
 */
inline static void
local_writeMask(makeMask_t mama);


/**
 * @brief  Generates the histogram of the mask and reports to amount of
 *         particles that are required.
 *
 * @param[in,out]  mama
 *                    The makeMask object to work with.
 *
 * @return  Returns nothing.
 */
inline static void
local_doHistogram(makeMask_t mama);


/**
 * @brief  Generates the shape that is used to generate the transition from
 *         high-resolution to low-resolution.
 *
 * The idea is to create a cubic shapelet that has in its center the highest
 * resolution and the surrounding cells are tagged according to their
 * distance to the center.  I.e. the outer shell will be the coarsest
 * resolution.
 *
 * As a side-effect, the function will write the shapelet in as binary data
 * into the file @c shape.dat.
 *
 * @param[in]  shapeDim1D
 *                The size of the shaplet, the actual shape will be
 *                (shapeDim1D / 2)+1, i.e. there will be shapeDim1D/2 cells
 *                on either side of the central cell.
 *
 * @return  Returns an array of size (shapeDim1D / 2 + 1)^NDIM holding the
 *          shapelet.
 */
inline static uint8_t *
local_createDegradeShape(uint32_t shapeDim1D);


/**
 * @brief  Helper function that performs the heavy-lifting in creating the
 *         mask.
 *
 * @param[in,out]  *maskData
 *                    The mask data attached to the patch of the grid of the
 *                    calling function.  This data must be correctly
 *                    described by @c idxLo and @dimsPatch.
 * @param[in]      hiResCellIdxG
 *                    The index of the cell the function should work on in
 *                    grid coordinates (*not* patch coordinate).
 * @param[in]      *shape
 *                    The shapelet as returned by
 *                    local_createDegradeShape().
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
local_throwShapeOnMask(int8_t                  *restrict maskData,
                       const gridPointUint32_t hiResCellIdxG,
                       const uint8_t           *restrict shape,
                       uint32_t                shapeDim1D,
                       const gridPointUint32_t idxLo,
                       const gridPointUint32_t dimsPatch,
                       const gridPointUint32_t dimsGrid);


/*--- Implementations of exported functios ------------------------------*/
extern makeMask_t
makeMask_newFromIni(parse_ini_t ini, const char *maskSectionName)
{
	makeMask_t mama;

	assert(ini != NULL);
	assert(maskSectionName != NULL);

	mama          = xmalloc(sizeof(struct makeMask_struct));

	mama->setup   = makeMaskSetup_newFromIni(ini, maskSectionName);
	mama->grid    = local_getGrid(mama->setup);
	mama->distrib = local_getDistrib(mama);
	mama->writer  = gridWriterFactory_newWriterFromIni(
	    ini,
	    mama->setup->outSecName);
#ifdef WITH_MPI
	gridWriter_initParallel(mama->writer, MPI_COMM_WORLD);
#endif

	return mama;
}

extern void
makeMask_run(makeMask_t mama)
{
	double timing;

	assert(mama != NULL);

	timing = timer_start_text("  Generating empty mask... ");
	local_createEmptyMask(mama);
	timing = timer_stop_text(timing, "took %.5fs\n");

	timing = timer_start_text("  Marking refinement levels... ");
	local_markRegions(mama);
	timing = timer_stop_text(timing, "took %.5fs\n");

	local_writeMask(mama);
	local_doHistogram(mama);
}

extern void
makeMask_del(makeMask_t *mama)
{
	assert(mama != NULL);
	assert(*mama != NULL);

	makeMaskSetup_del(&((*mama)->setup));
	gridRegularDistrib_del(&((*mama)->distrib));
	gridRegular_del(&((*mama)->grid));
	gridWriter_del(&((*mama)->writer));
	xfree(*mama);

	*mama = NULL;
}

/*--- Implementations of local functions --------------------------------*/
inline static gridRegular_t
local_getGrid(makeMaskSetup_t setup)
{
	gridPointDbl_t    origin, extent;
	gridPointUint32_t dims;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = (double)(setup->baseGridSize1D);
		dims[i]   = setup->baseGridSize1D;
	}

	return gridRegular_new("MaskGrid", origin, extent, dims);
}

inline static gridRegularDistrib_t
local_getDistrib(const makeMask_t mama)
{
	gridRegularDistrib_t distrib;

	distrib = gridRegularDistrib_new(mama->grid, NULL);
#ifdef WITH_MPI
	gridRegularDistrib_initMPI(distrib,
	                           mama->setup->nProcs, MPI_COMM_WORLD);
#endif

	return distrib;
}

inline static void
local_createEmptyMask(makeMask_t mama)
{
	int         rank = 0;
	gridPatch_t patch;
	dataVar_t   var  = dataVar_new("Mask", DATAVARTYPE_INT8, 1);
	int8_t      *maskData;
	int8_t      emptyValue;
	uint64_t    numCells;

#ifdef WITH_MPI
	rank  = gridRegularDistrib_getLocalRank(mama->distrib);
#endif
	patch = gridRegularDistrib_getPatchForRank(mama->distrib, rank);
	gridRegular_attachPatch(mama->grid, patch);
	gridRegular_attachVar(mama->grid, var);

	maskData   = gridPatch_getVarDataHandle(patch, 0);

	numCells   = gridPatch_getNumCellsActual(patch, 0);
	emptyValue = (int8_t)(mama->setup->baseRefinementLevel);
#ifdef WITH_OPENMP
#  pragma omp parallel for shared(maskData, numCells, emptyValue) \
	schedule(static)
#endif
	for (uint64_t i = 0; i < numCells; i++) {
		maskData[i] = emptyValue;
	}
}

inline static void
local_markRegions(makeMask_t mama)
{
	gridPointUint32_t dimsGrid, dimsPatch, idxLo;
	gridPatch_t       patch;
	int8_t            *maskData;
	uint32_t          shapeDim1D;
	uint8_t           *shape;

	gridRegular_getDims(mama->grid, dimsGrid);
	patch = gridRegular_getPatchHandle(mama->grid, 0);
	gridPatch_getDims(patch, dimsPatch);
	gridPatch_getIdxLo(patch, idxLo);
	maskData   = gridPatch_getVarDataHandle(patch, 0);

	shapeDim1D = 2 * (mama->setup->numLevels - 2) + 1;
	shape      = local_createDegradeShape(shapeDim1D);

	for (uint32_t i = 0; i < lare_getNumElements(mama->setup->lare); i++) {
		gridPointUint32_t element;
		lare_getElement(mama->setup->lare, element, i);
		local_throwShapeOnMask(maskData, element, shape, shapeDim1D,
		                       idxLo, dimsPatch, dimsGrid);
	}
}

inline static void
local_writeMask(makeMask_t mama)
{
	double timing;

	timing = timer_start_text("  Writing mask to file... ");
	gridWriter_activate(mama->writer);
	gridWriter_writeGridRegular(mama->writer, mama->grid);
	gridWriter_deactivate(mama->writer);
	timing = timer_stop_text(timing, "took %.5fs\n");
}

inline static void
local_doHistogram(makeMask_t mama)
{
	double          timing;
	gridHistogram_t histo;
	int             rank = 0;

#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	timing = timer_start_text("  Generating histogram... ");
	histo  = gridHistogram_new(mama->setup->numLevels, -0.5,
	                           mama->setup->numLevels - 0.5);
	gridHistogram_calcGridRegularDistrib(histo, mama->distrib, 0);
	timing = timer_stop(timing);
	timing = timer_stop_text(timing, "took %.5fs\n");
	if (rank == 0) {
		uint64_t numTotalParts = 0;
		int      particleMult  = 1;
		int      gridMult      = 1;
		for (uint32_t i = 0; i < mama->setup->numLevels; i++) {
			uint32_t numInBin = gridHistogram_getCountInBin(histo, i + 1);
			printf("    level %1u (%5u^%i): %10u (%10u particles)\n",
			       i, mama->setup->baseGridSize1D * gridMult, NDIM,
			       numInBin, numInBin * particleMult);
			numTotalParts += numInBin * particleMult;
			particleMult  *= POW_NDIM(mama->setup->refinementFactor);
			gridMult      *= mama->setup->refinementFactor;
		}
		printf("      -->  total of %10lu particles\n", numTotalParts);
	}

	gridHistogram_del(&histo);
}

inline static uint8_t *
local_createDegradeShape(uint32_t shapeDim1D)
{
	uint8_t *shape;
	int     extent = shapeDim1D / 2;
	int     idx    = 0;
	FILE    *f;

	shape = xmalloc(sizeof(uint8_t) * (POW_NDIM(extent * 2 + 1)));

	for (int k = -extent; k <= extent; k++) {
		for (int j = -extent; j <= extent; j++) {
			for (int i = -extent; i <= extent; i++) {
				int dist = (int)floor(sqrt(k * k + j * j + i * i));
				shape[idx++] = (uint8_t)(dist < extent + 1
				                         ? extent + 1 - dist : 0);
			}
		}
	}

	f = fopen("shape.dat", "wb");
	fwrite(shape, sizeof(int8_t), (POW_NDIM(extent * 2 + 1)), f);
	fclose(f);

	return shape;
}

inline static void
local_throwShapeOnMask(int8_t                  *restrict maskData,
                       const gridPointUint32_t hiResCellIdxG,
                       const uint8_t           *restrict shape,
                       uint32_t                shapeDim1D,
                       const gridPointUint32_t idxLo,
                       const gridPointUint32_t dimsPatch,
                       const gridPointUint32_t dimsGrid)
{
	uint32_t iSG, jSG, kSG; // position of shape (S) in grid coordinates (G)
	int32_t  iSM, jSM, kSM; // position of shape (S) in map coordinates (M)
	int32_t  shapeExtent = shapeDim1D / 2;
	uint64_t idxS, idxM;

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
}
