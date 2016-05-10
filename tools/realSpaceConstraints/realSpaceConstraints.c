// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file realSpaceConstraints/realSpaceConstraints.c
 * @ingroup  toolsRealSpaceConstraints
 * @brief  Provides the implementation of the realSpaceConstraints tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "realSpaceConstraintsConfig.h"
#include "realSpaceConstraints.h"
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#ifdef WITH_OPENMP
#  include <omp.h>
#endif
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"
#include "../../src/libgrid/gridStatistics.h"
#include "../../src/libgrid/gridReaderFactory.h"
#include "../../src/libgrid/gridWriterFactory.h"
#include "../../src/libgrid/gridPatch.h"
#include "../../src/libgrid/gridHistogram.h"
#include "../../src/libdata/dataVar.h"
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/timer.h"
#include "../../src/libutil/rng.h"
#include "../../src/libutil/tile.h"


/*--- Implemention of main structure ------------------------------------*/
#include "realSpaceConstraints_adt.h"


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Gets the extent of the grid the process holds of the input grid.
 *
 * @param[in]   inputDim1D
 *                 The dimensions of the input grid.
 * @param[in]   outputDim1D
 *                 The dimensions of the output grid.
 * @param[out]  lastDimLimits
 *                 An array of @c 2 elements that will receive the first and
 *                 the last index of the last dimension (z-Dimension for 3D
 *                 cases).
 *
 * @return  Returns nothing.
 */
static void
local_getLastDimLimitsInput(uint32_t inputDim1D,
                            uint32_t outputDim1D,
                            uint32_t lastDimLimits[2]);


/**
 * @brief  Gets the extent of the grid the process holds of the output grid.
 *
 * @param[in]   inputDim1D
 *                 The dimensions of the input grid.
 * @param[in]   outputDim1D
 *                 The dimensions of the output grid.
 * @param[out]  lastDimLimits
 *                 An array of @c 2 elements that will receive the first and
 *                 the last index of the last dimension (z-Dimension for 3D
 *                 cases).
 *
 * @return  Returns nothing.
 */
static void
local_getLastDimLimitsOutput(uint32_t inputDim1D,
                             uint32_t outputDim1D,
                             uint32_t lastDimLimits[2]);


/**
 * @brief  Gets a simple grid.
 *
 * @param[in]  boxsizeInMpch
 *                The boxsize in Mpc/h.
 * @param[in]  dim1D
 *                The grid size.
 * @param[in]  lastDimLimits
 *                Provides the extent of the last dimension for the patch
 *                attached to the grid.
 * @param[in]  *name
 *                The grid name.
 *
 * @return  Returns a new grid.
 */
static gridRegular_t
local_getGrid(double         boxsizeInMpch,
              uint32_t       dim1D,
              const uint32_t lastDimLimits[2],
              const char     *name);


/**
 * @brief  Fill the input grid, either from a file or via RNG.
 *
 * @param[in,out]  grid
 *                    The grid to work with.
 * @param[in,out]  reader
 *                    The reader to use.  If this is @c NULL, the grid will
 *                    be filled via the RNG.
 * @param[in]      seed
 *                    The seed for the RNG, only used if @c reader is
 *                    @c NULL.
 *
 * @return  Returns nothing.
 */
static void
local_fillInputGrid(gridRegular_t grid, gridReader_t reader, int seed);


/**
 * @brief  Fills the output grid from the input grid.
 *
 * This will either generate a grid with the RNG and then impose the
 * constraints of the input grid (output grid is larger than the input
 * grid), or it will degrade the input grid to the resolution of the output
 * grid.
 *
 * @param[in,out]  gridOut
 *                    The output grid that will be filled by this routine.
 * @param[in]      gridIn
 *                    The input grid that will be used as constraints (if it
 *                    is smaller than the output grid) or degraded (if it is
 *                    larger than the input grid).
 * @param[in]      seedOut
 *                    The seed for the output grid.  This is only used, if
 *                    the input grid is smaller than the output grid.
 *
 * @return  Returns nothing.
 */
static void
local_fillOutputGrid(gridRegular_t       gridOut,
                     const gridRegular_t gridIn,
                     int                 seedOut);


/**
 * @brief  This will simply fill a patch with white noise.
 *
 * @param[in,out]  patch
 *                    The patch to be filled.
 * @param[in]      seed
 *                    The seed that should be used for the RNG.
 *
 * @return  Returns nothing.
 */
static void
local_fillPatchWithWhiteNoise(gridPatch_t patch, int seed);


/**
 * @brief  Does the heavy lifting for degrading a data cube.
 *
 *
 * General conversion from @f$(i,j,k)@f$ to linear array index (assuming
 * array has the dimensions@f$(d_0, d_1, d_2)@f$):
 * @f[
 *   idx(i, j, k) = i + (j + k d_1) d_0
 * @f]
 *
 * Given two grids g_H and g_L with @f$(d_{H,0}, d_{H,1}, d_{H,2}) =
 * (f_0 d_{L,0}, f_1 d_{L,1}, f_2 d_{L,2})@f$, the position in the H
 * grid can be calculated from the coordinates of the L grid and a
 * position in the subvolume as
 * @f[
 *   i_H = i_L f_0 + i_S \, j_H = j_L f_1 + j_S \, k_H = k_L f_2 + k_S
 * @f]
 * which leads to the expression for the linear index in the H grid as
 * @f[
 *  i_H + (j_H + k_H d_{H,1}) d_{H,0}
 *   = i_L f_0 + (j_L f_1 + k_L f_2 d_{H,1}) d_{H,0}
 *     + i_S + (j_S + k_S d_{H,1}) d_{H,0}
 * @f]
 *
 * @param[out]  *dataOut
 *                 The output data cube.
 * @param[in]   *dataIn
 *                 The input data cube.
 * @param[in]   dimsOut
 *                 The dimensions of the output data cube.
 * @param[in]   dimsIn
 *                 The dimensions of the input data cube.
 *
 * @return  Returns nothing.
 */
static void
local_degrade(fpv_t             *dataOut,
              const fpv_t       *dataIn,
              gridPointUint32_t dimsOut,
              gridPointUint32_t dimsIn);


/**
 * @brief  Will enforce constraints of a lowRes grid onto a highRes grid.
 *
 * @param[out]  *dataOut
 *                 The output data cube.
 * @param[in]   *dataIn
 *                 The input data cube.
 * @param[in]   dimsOut
 *                 The dimensions of the output data cube.
 * @param[in]   dimsIn
 *                 The dimensions of the input data cube.
 *
 * @return  Returns nothing.
 */
static void
local_enforceConstraints(fpv_t             *dataOut,
                         const fpv_t       *dataIn,
                         gridPointUint32_t dimsOut,
                         gridPointUint32_t dimsIn);


inline static void
local_fillCoeff(const fpv_t       *dataIn,
		const fpv_t       *dataOut,
		gridPointUint32_t dimsIn,
		gridPointUint32_t dimsOut,
		double		*a);

inline static void
local_refine(fpv_t       *data,
		gridPointUint32_t dims,
		const double		*a);

/**
 * @brief  This will sum over a subvolume.
 *
 * @param[in]  *data
 *                The data.  Must point to the beginning of the subvolume.
 * @param[in]  dimsIn
 *                The dimension of the input grid.
 * @param[in]  dimsSV
 *                The dimension of the subvolume.
 *
 * @return  Returns the sum of all values in the subvolume.
 */
inline static long double
local_sumSV(const fpv_t       *data,
            gridPointUint32_t dimsIn,
            gridPointUint32_t dimsSV);


/**
 * @brief  Adds a scalar value to a subvolume.
 *
 * @param[in,out]  *data
 *                    The data, must point to the beginning of the
 *                    subvolume.
 * @param[in]      dimsIn
 *                    The dimensions of the input data.
 * @param[in]      dimsSV
 *                    The dimensions of the subvolume.
 * @param[in]      value
 *                    The value that should be added to the subvolume.
 *
 * @return  Returns nothing.
 */
inline static void
local_addToSV(fpv_t             *data,
              gridPointUint32_t dimsIn,
              gridPointUint32_t dimsSV,
              double            value);


/*--- Implementations of exported functios ------------------------------*/
extern realSpaceConstraints_t
realSpaceConstraints_newFromIni(parse_ini_t ini)
{
	realSpaceConstraints_t te;
	uint32_t               lastDimLimits[2];

	assert(ini != NULL);

	te        = xmalloc(sizeof(struct realSpaceConstraints_struct));

	te->setup = realSpaceConstraintsSetup_newFromIni(ini,
	                                                 "Setup");
	local_getLastDimLimitsInput(te->setup->inputDim1D,
	                            te->setup->outputDim1D,
	                            lastDimLimits);
	te->gridIn = local_getGrid(te->setup->boxsizeInMpch,
	                           te->setup->inputDim1D,
	                           lastDimLimits,
	                           "Input");
	local_getLastDimLimitsOutput(te->setup->inputDim1D,
	                             te->setup->outputDim1D,
	                             lastDimLimits);
	te->gridOut = local_getGrid(te->setup->boxsizeInMpch,
	                            te->setup->outputDim1D,
	                            lastDimLimits,
	                            "Output");
	if (te->setup->useFileForInput) {
		te->reader   = gridReaderFactory_newReaderFromIni(
		    ini, te->setup->readerSecName);
		te->writerIn = NULL;
	} else {
		te->reader   = NULL;
		te->writerIn = gridWriterFactory_newWriterFromIni(
		    ini, te->setup->writerInSecName);
#ifdef WITH_MPI
		gridWriter_initParallel(te->writerIn, MPI_COMM_WORLD);
#endif
	}
	te->writer = gridWriterFactory_newWriterFromIni(
	    ini, te->setup->writerSecName);
#ifdef WITH_MPI
	gridWriter_initParallel(te->writer, MPI_COMM_WORLD);
#endif

	return te;
} /* realSpaceConstraints_newFromIni */

extern void
realSpaceConstraints_run(realSpaceConstraints_t te)
{
	double           timing;
	gridStatistics_t stat;
	gridRegularDistrib_t distrib;
	int              rank = 0;
	gridPointInt_t nProc;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	assert(te != NULL);

	nProc[0] = 1;
	nProc[1] = 1;
	nProc[2] = 0;

	stat   = gridStatistics_new();

	timing = timer_start_text("  Filling input grid... ");
	local_fillInputGrid(te->gridIn, te->reader, te->setup->seedIn);
	timing = timer_stop_text(timing, "took %.5fs\n");

	timing = timer_start_text("  Calculating statistics on input grid... ");
#ifdef WITH_MPI	
	distrib = gridRegularDistrib_new(te->gridIn, NULL);
	gridRegularDistrib_initMPI(distrib, nProc, MPI_COMM_WORLD);
	gridStatistics_calcGridRegularDistrib(stat, distrib, 0);
#else
	gridStatistics_calcGridRegular(stat, te->gridIn, 0);
#endif
	timing = timer_stop_text(timing, "took %.5fs\n");
	if (rank == 0)
		gridStatistics_printPretty(stat, stdout, "  ");

	if (te->writerIn != NULL) {
		timing = timer_start_text("  Writing input grid to file... ");
		gridWriter_activate(te->writerIn);
		gridWriter_writeGridRegular(te->writerIn, te->gridIn);
		gridWriter_deactivate(te->writerIn);
		timing = timer_stop_text(timing, "took %.5fs\n");
	}

	timing = timer_start_text("  Filling output grid... ");
	local_fillOutputGrid(te->gridOut, te->gridIn, te->setup->seedOut);
	timing = timer_stop_text(timing, "took %.5fs\n");

	timing = timer_start_text("  Calculating statistics on output grid... ");
#ifdef WITH_MPI 
        distrib = gridRegularDistrib_new(te->gridOut, NULL);
        gridRegularDistrib_initMPI(distrib, nProc, MPI_COMM_WORLD);
        gridStatistics_calcGridRegularDistrib(stat, distrib, 0);
#else
	gridStatistics_calcGridRegular(stat, te->gridOut, 0);
#endif
	timing = timer_stop_text(timing, "took %.5fs\n");
	if (rank == 0)
		gridStatistics_printPretty(stat, stdout, "  ");

	timing = timer_start_text("  Writing output grid to file... ");
	gridWriter_activate(te->writer);
	gridWriter_writeGridRegular(te->writer, te->gridOut);
	gridWriter_deactivate(te->writer);
	timing = timer_stop_text(timing, "took %.5fs\n");

	gridStatistics_del(&stat);
} /* realSpaceConstraints_run */

extern void
realSpaceConstraints_del(realSpaceConstraints_t *te)
{
	assert(te != NULL);
	assert(*te != NULL);

	realSpaceConstraintsSetup_del(&((*te)->setup));
	gridRegular_del(&((*te)->gridIn));
	gridRegular_del(&((*te)->gridOut));
	gridWriter_del(&((*te)->writer));
	if ((*te)->writerIn != NULL)
		gridWriter_del(&((*te)->writerIn));
	if ((*te)->reader != NULL)
		gridReader_del(&((*te)->reader));
	xfree(*te);

	*te = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_getLastDimLimitsInput(uint32_t inputDim1D,
                            uint32_t outputDim1D,
                            uint32_t lastDimLimits[2])
{
	int tile     = 0;
	int numTiles = 1;
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &numTiles);
	MPI_Comm_rank(MPI_COMM_WORLD, &tile);
#endif

	if (inputDim1D <= outputDim1D) {
		tile_calcIdxsELAE(inputDim1D, numTiles, tile,
		                  lastDimLimits, lastDimLimits + 1);
	} else {
		assert(inputDim1D % outputDim1D == 0);
		int factor = inputDim1D / outputDim1D;
		tile_calcIdxsELAE(outputDim1D, numTiles, tile,
		                  lastDimLimits, lastDimLimits + 1);
		lastDimLimits[0] *= factor;
		lastDimLimits[1]  = ((lastDimLimits[1] + 1) * factor) - 1;
	}
}

static void
local_getLastDimLimitsOutput(uint32_t inputDim1D,
                             uint32_t outputDim1D,
                             uint32_t lastDimLimits[2])
{
	int tile     = 0;
	int numTiles = 1;
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &numTiles);
	MPI_Comm_rank(MPI_COMM_WORLD, &tile);
#endif

	if (outputDim1D <= inputDim1D) {
		tile_calcIdxsELAE(outputDim1D, numTiles, tile,
		                  lastDimLimits, lastDimLimits + 1);
	} else {
		assert(outputDim1D % inputDim1D == 0);
		int factor = outputDim1D / inputDim1D;
		tile_calcIdxsELAE(inputDim1D, numTiles, tile,
		                  lastDimLimits, lastDimLimits + 1);
		lastDimLimits[0] *= factor;
		lastDimLimits[1]  = ((lastDimLimits[1] + 1) * factor) - 1;
	}
}

static gridRegular_t
local_getGrid(double         boxsizeInMpch,
              uint32_t       dim1D,
              const uint32_t lastDimLimits[2],
              const char     *name)
{
	assert(lastDimLimits[0] <= lastDimLimits[1]);
	assert(lastDimLimits[1] < dim1D);

	gridPointDbl_t    origin, extent;
	gridPointUint32_t dims;
	gridRegular_t     grid;
	gridPatch_t       patch;
	gridPointUint32_t idxLo;
	gridPointUint32_t idxHi;
	dataVar_t         var;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = (double)(boxsizeInMpch);
		dims[i]   = dim1D;
		idxLo[i]  = (i < NDIM - 1) ? 0 : lastDimLimits[0];
		idxHi[i]  = (i < NDIM - 1) ? dim1D - 1 : lastDimLimits[1];
	}
	grid  = gridRegular_new(name, origin, extent, dims);

	patch = gridPatch_new(idxLo, idxHi);
	gridRegular_attachPatch(grid, patch);

	var = dataVar_new("wn", DATAVARTYPE_FPV, 1);
	gridRegular_attachVar(grid, var);

	return grid;
}

static void
local_fillInputGrid(gridRegular_t grid, gridReader_t reader, int seed)
{
	gridPatch_t patch;

	patch = gridRegular_getPatchHandle(grid, 0);

	if (reader == NULL) {
		local_fillPatchWithWhiteNoise(patch, seed);
	} else {
		gridReader_readIntoPatchForVar(reader, patch, 0);
	}
}

static void
local_fillOutputGrid(gridRegular_t       gridOut,
                     const gridRegular_t gridIn,
                     int                 seedOut)
{
	gridPatch_t       patchIn, patchOut;
	fpv_t             *dataIn, *dataOut;
	gridPointUint32_t dimsIn, dimsOut;
//	gridPointUint32_t factors;

	patchIn = gridRegular_getPatchHandle(gridIn, 0);
	dataIn  = (fpv_t *)gridPatch_getVarDataHandle(patchIn, 0);
	gridPatch_getDims(patchIn, dimsIn);

	patchOut = gridRegular_getPatchHandle(gridOut, 0);
	dataOut  = (fpv_t *)gridPatch_getVarDataHandle(patchOut, 0);
	gridPatch_getDims(patchOut, dimsOut);

	if ((dimsIn[0] < dimsOut[0]) && (dimsIn[1] < dimsOut[1])
	    && (dimsIn[2] < dimsOut[2])) {
		local_fillPatchWithWhiteNoise(patchOut, seedOut);
		local_enforceConstraints(dataOut, dataIn, dimsOut, dimsIn);
	} else if ((dimsIn[0] > dimsOut[0]) && (dimsIn[1] > dimsOut[1])
	           && (dimsIn[2] > dimsOut[2])) {
		local_degrade(dataOut, dataIn, dimsOut, dimsIn);
	} else {
		fprintf(stdout, "doing nothing");
	}
}

static void
local_fillPatchWithWhiteNoise(gridPatch_t patch, int seed)
{
	fpv_t    *data;
	uint64_t numCells;
	rng_t    rng;
	int      size       = 1;
	int      rank       = 0;
	int      numThreads = 1;
	int      numStreams = 1;
#ifdef WITH_OPENMP
	numThreads = omp_get_max_threads();
#endif
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	numStreams = numThreads * size;

	data       = (fpv_t *)gridPatch_getVarDataHandle(patch, 0);
	numCells   = gridPatch_getNumCells(patch);
	rng        = rng_new(4, numStreams, seed);

#ifdef _OPENMP
#  pragma omp parallel for shared(data, numThreads, numCells)
#endif
	for (int i = 0; i < numThreads; i++) {
		uint64_t cps   = numCells / numThreads;
		uint64_t start = i * cps;
		uint64_t stop  = (i == numStreams - 1) ? numCells : (start
		                                                     + cps);
		for (uint64_t j = start; j < stop; j++) {
			assert(j < numCells);
			data[j] = (fpv_t)rng_getGaussUnit(rng, i);
		}
	}

	rng_del(&rng);
}

static void
local_degrade(fpv_t             *dataOut,
              const fpv_t       *dataIn,
              gridPointUint32_t dimsOut,
              gridPointUint32_t dimsIn)
{
	gridPointUint32_t dimsSV;
	double            numCellsSVInv      = 1.;
	double            varianceAdjustment = 1;
	//TODO rewrite degrading using Jetkins algorithm

	for (int i = 0; i < NDIM; i++) {
		assert(dimsIn[i] % dimsOut[i] == 0);
		dimsSV[i]           = dimsIn[i] / dimsOut[i];
		numCellsSVInv      /= (double)(dimsSV[i]);
		varianceAdjustment *= dimsIn[i] / ((double)(dimsOut[i]));
	}
	varianceAdjustment = sqrt(varianceAdjustment);

#if (NDIM > 2)
#  ifdef WITH_OPENMP
#    pragma omp parallel for
#  endif
	for (uint64_t k = 0; k < dimsOut[2]; k++)
#endif
	{
		for (uint64_t j = 0; j < dimsOut[1]; j++) {
			for (uint64_t i = 0; i < dimsOut[0]; i++) {
				long double sum;
				uint64_t    idxOut = i + (j + k * dimsOut[1]) * dimsOut[0];
				uint64_t    idxIn  = i * dimsSV[0]
				                     + (j * dimsSV[1]
				                        + k * dimsSV[2]
				                        * dimsIn[1]) * dimsIn[0];
				sum = local_sumSV(dataIn + idxIn,
				                  dimsIn,
				                  dimsSV);
				dataOut[idxOut] = (fpv_t)(sum * numCellsSVInv
				                          * varianceAdjustment);
			}
		}
	}
} /* local_degrade */

static void
local_enforceConstraints(fpv_t             *dataOut,
                         const fpv_t       *dataIn,
                         gridPointUint32_t dimsOut,
                         gridPointUint32_t dimsIn)
{
	gridPointUint32_t dimsSV;

	for (int i = 0; i < NDIM; i++) {
		assert(dimsOut[i] % dimsIn[i] == 0);
		dimsSV[i]           = dimsOut[i] / dimsIn[i];
		assert(dimsSV[i]==2);
	}

#if (NDIM > 2)
#  ifdef WITH_OPENMP
#    pragma omp parallel for
#  endif
	for (uint64_t k = 0; k < dimsIn[2]; k+=2)
#endif
	{
		for (uint64_t j = 0; j < dimsIn[1]; j+=2) {
			for (uint64_t i = 0; i < dimsIn[0]; i+=2) {
				uint64_t    idxIn  = i + (j + k * dimsIn[1]) * dimsIn[0];
				uint64_t    idxOut = i * dimsSV[0]
				                     + (j * dimsSV[1]
				                        + k * dimsSV[2]
				                        * dimsOut[1]) * dimsOut[0];
				double			  a[64]; // expansion coefficients

				local_fillCoeff(dataIn+idxIn,dataOut+idxOut,dimsIn,dimsOut,a);

				local_refine(dataOut+idxOut,dimsOut,a);

			}
		}
	}
} /* local_enforceConstraints */

inline static void
local_fillCoeff(const fpv_t       *dataIn,
		const fpv_t       *dataOut,
		gridPointUint32_t dimsIn,
		gridPointUint32_t dimsOut,
		double		*a)
{
	int i,j,k,ii,jj,kk;
	const double mat[8][8] = {{1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0},{-1.0,1.0,-1.0,1.0,-1.0,1.0,-1.0,1.0},{-1.0,-1.0,1.0,1.0,-1.0,-1.0,1.0,1.0},{1.0,-1.0,-1.0,1.0,1.0,-1.0,-1.0,1.0},{-1.0,-1.0,-1.0,-1.0,1.0,1.0,1.0,1.0},{1.0,-1.0,1.0,-1.0,-1.0,1.0,-1.0,1.0},{1.0,1.0,-1.0,-1.0,-1.0,-1.0,1.0,1.0},{-1.0,1.0,1.0,-1.0,1.0,-1.0,-1.0,1.0}};
	// initially fill a[][][] with random numbers from dataOut:
	for (k=0; k<4; k++)
		for (j=0; j<4; j++)
			for (i=0; i<4; i++) {
				a[i+(j+k*4)*4] = dataOut[i + (j + k * dimsOut[1]) * dimsOut[0]];
			}
	// put constraints on a[0..1][0..1][0..1]:
	for (k=0; k<2; k++)
		for (j=0; j<2; j++)
			for (i=0; i<2; i++) {
				a[i+(j+k*4)*4] = 0;
				for (kk=0; kk<2; kk++)
					for (jj=0; jj<2; jj++)
						for (ii=0; ii<2; ii++)
							a[i+(j+k*4)*4] += dataIn[ii+(jj+kk*dimsIn[1])*dimsIn[0]] * mat[2*(2*k+j)+i][2*(2*kk+jj)+ii]/sqrt(8.0);
			}
}

inline static void
local_refine(fpv_t       *data,
		gridPointUint32_t dims,
		const double		*a)
{
	int i,j,k,ii,jj,kk;
	double e[4][4]={{0.5,-0.661437827766,-0.5,0.25},{0.5,-0.25,0.5,-0.661437827766},{0.5,0.25,0.5,0.661437827766},{0.5,0.661437827766,-0.5,-0.25}};
	for (k=0; k<4; k++)
			for (j=0; j<4; j++)
				for (i=0; i<4; i++) {
					data[i+(j+k*dims[1])*dims[0]] = 0;
					for (kk=0; kk<4; kk++)
						for (jj=0; jj<4; jj++)
							for (ii=0; ii<4; ii++)
								data[i+(j+k*dims[1])*dims[0]] += a[ii+(jj+kk*4)*4] * e[k][kk] * e[j][jj] * e[i][ii];
				}
}

inline static long double
local_sumSV(const fpv_t       *data,
            gridPointUint32_t dimsIn,
            gridPointUint32_t dimsSV)
{
	long double sum = 0.0L;

#if (NDIM > 2)
	for (uint64_t k = 0; k < dimsSV[2]; k++)
#endif
	{
		for (uint64_t j = 0; j < dimsSV[1]; j++) {
			for (uint64_t i = 0; i < dimsSV[0]; i++) {
				sum += data[i + (j + k * dimsIn[1]) * dimsIn[0]];
			}
		}
	}


	return sum;
}

inline static void
local_addToSV(fpv_t             *data,
              gridPointUint32_t dimsIn,
              gridPointUint32_t dimsSV,
              double            value)
{
#if (NDIM > 2)
	for (uint64_t k = 0; k < dimsSV[2]; k++)
#endif
	{
		for (uint64_t j = 0; j < dimsSV[1]; j++) {
			for (uint64_t i = 0; i < dimsSV[0]; i++) {
				data[i + (j + k * dimsIn[1]) * dimsIn[0]] += (fpv_t)value;
			}
		}
	}
}
