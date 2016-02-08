// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file refineGrid/refineGrid.c
 * @ingroup  toolsrefineGrid
 * @brief  Provides the implementation of the refineGrid tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "refineGridConfig.h"
#include "refineGrid.h"
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

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define NBR(x,dx,dim) ( MIN(MAX(((x)+(dx)-1),0),((dim)-1) ) )

/*--- Implemention of main structure ------------------------------------*/
#include "refineGrid_adt.h"


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
 * @param[in]  *varName
 *                The variable name.
 *
 * @return  Returns a new grid.
 */
static gridRegular_t
local_getGrid(double         boxsizeInMpch,
              uint32_t       dim1D,
              const uint32_t lastDimLimits[2],
              const char     *name,
              const char	 *varName);


/**
 * @brief  Fill the input grid, either from a file or via RNG.
 *
 * @param[in,out]  grid
 *                    The grid to work with.
 * @param[in,out]  reader
 *                    The reader to use.
 *
 * @return  Returns nothing.
 */
static void
local_fillInputGrid(gridRegular_t grid, gridReader_t reader);


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
 *                    is smaller than the output grid).
 * @param[in]      gridIn2
 *                    The second input grid that will be added to the results if not 
 *                    @c NULL.
 *
 * @return  Returns nothing.
 */
static void
local_fillOutputGrid(gridRegular_t       gridOut,
                     const gridRegular_t gridIn,
                     const gridRegular_t gridIn2);

static void
local_dataCopy(fpv_t            *dataOut,
              const fpv_t       *dataIn,
              gridPointUint32_t dimsOut,
              gridPointUint32_t dimsIn);

/**
 * @brief  Will add input2 grid to output grid.
 *
 * @param[out]  *dataOut
 *                 The output data cube.
 * @param[in]   *dataIn
 *                 The input data cube.
 * @param[in]   dimsOut
 *                 The dimensions of the both data cubes.
 *
 * @return  Returns nothing.
 */
static void
local_addGrid(fpv_t             *dataOut,
              const fpv_t       *dataIn,
              gridPointUint32_t dimsOut);

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


/**
 * @brief  Sets a scalar value to a subvolume.
 *
 * @param[in,out]  *data
 *                    The data, must point to the beginning of the
 *                    subvolume.
 * @param[in]      dimsIn
 *                    The dimensions of the input data.
 * @param[in]      dimsSV
 *                    The dimensions of the subvolume.
 * @param[in]      value
 *                    The value that should be set to the subvolume.
 *
 * @return  Returns nothing.
 */
inline static void
local_setToSV(fpv_t             *data,
              gridPointUint32_t dimsIn,
              gridPointUint32_t dimsSV,
              double            value);
              
/**
 * @brief  Interpolates values to a subvolume.
 *
 * @param[in,out]  *data
 *                    The data, must point to the beginning of the
 *                    subvolume.
 * @param[in]      dimsIn
 *                    The dimensions of the input data.
 * @param[in]      dimsSV
 *                    The dimensions of the subvolume.
 * @param[in]      *valArr
 *                    The value array of size 8 (2x2x2) that should be interpolated to the subvolume.
 *
 * @return  Returns nothing.
 */           
inline static void
local_CICToSV(fpv_t             *data,
              gridPointUint32_t dimsIn,
              gridPointUint32_t dimsSV,
              double 			*valArr);


/*--- Implementations of exported functios ------------------------------*/
extern refineGrid_t
refineGrid_newFromIni(parse_ini_t ini)
{
	refineGrid_t te;
	uint32_t               lastDimLimits[2];

	assert(ini != NULL);

	te        = xmalloc(sizeof(struct refineGrid_struct));

	te->setup = refineGridSetup_newFromIni(ini,
	                                                 "Setup");
	local_getLastDimLimitsInput(te->setup->inputDim1D,
	                            te->setup->outputDim1D,
	                            lastDimLimits);
	te->gridIn = local_getGrid(te->setup->boxsizeInMpch,
	                           te->setup->inputDim1D,
	                           lastDimLimits,
	                           "Input",
	                           te->setup->varName);
	local_getLastDimLimitsOutput(te->setup->inputDim1D,
	                             te->setup->outputDim1D,
	                             lastDimLimits);
	te->gridOut = local_getGrid(te->setup->boxsizeInMpch,
	                            te->setup->outputDim1D,
	                            lastDimLimits,
	                            "Output",
	                            te->setup->varName);
	te->reader   = gridReaderFactory_newReaderFromIni(
		    ini, te->setup->readerSecName);
	te->writer = gridWriterFactory_newWriterFromIni(
	    ini, te->setup->writerSecName);
#ifdef WITH_MPI
	gridWriter_initParallel(te->writer, MPI_COMM_WORLD);
#endif

    if(te->setup->addFields) {
		te->gridIn2 = local_getGrid(te->setup->boxsizeInMpch,
	                           te->setup->outputDim1D,
	                           lastDimLimits,
	                           "Input2",
	                           te->setup->varName);
	    te->reader2   = gridReaderFactory_newReaderFromIni(
		    ini, te->setup->reader2SecName);
	} else {
		te->gridIn2 = NULL;
		te->reader2 = NULL;
	}

	return te;
} /* refineGrid_newFromIni */

extern void
refineGrid_run(refineGrid_t te)
{
	double           timing;
	gridStatistics_t stat;
	gridRegularDistrib_t distrib;
	int              rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	assert(te != NULL);

	stat   = gridStatistics_new();

	timing = timer_start_text("  Filling input grid... ");
	local_fillInputGrid(te->gridIn, te->reader);
	timing = timer_stop_text(timing, "took %.5fs\n");

	timing = timer_start_text("  Calculating statistics on input grid... ");
#ifdef WITH_MPI	
	distrib = gridRegularDistrib_new(te->gridIn, NULL);
	gridRegularDistrib_initMPI(distrib, NULL, MPI_COMM_WORLD);
	gridStatistics_calcGridRegularDistrib(stat, distrib, 0);
#else
	gridStatistics_calcGridRegular(stat, te->gridIn, 0);
#endif
	timing = timer_stop_text(timing, "took %.5fs\n");
	if (rank == 0)
		gridStatistics_printPretty(stat, stdout, "  ");

    if(te->setup->addFields) {
		timing = timer_start_text("  Filling second input grid... ");
		local_fillInputGrid(te->gridIn2, te->reader2);
		timing = timer_stop_text(timing, "took %.5fs\n");
	
		timing = timer_start_text("  Calculating statistics on second input grid... ");
#ifdef WITH_MPI	
		distrib = gridRegularDistrib_new(te->gridIn2, NULL);
		gridRegularDistrib_initMPI(distrib, NULL, MPI_COMM_WORLD);
		gridStatistics_calcGridRegularDistrib(stat, distrib, 0);
#else
		gridStatistics_calcGridRegular(stat, te->gridIn2, 0);
#endif
		timing = timer_stop_text(timing, "took %.5fs\n");
		if (rank == 0)
			gridStatistics_printPretty(stat, stdout, "  ");
	}

	timing = timer_start_text("  Filling output grid... ");
	local_fillOutputGrid(te->gridOut, te->gridIn, te->gridIn2);
	timing = timer_stop_text(timing, "took %.5fs\n");

	timing = timer_start_text("  Calculating statistics on output grid... ");
#ifdef WITH_MPI 
        distrib = gridRegularDistrib_new(te->gridOut, NULL);
        gridRegularDistrib_initMPI(distrib, NULL, MPI_COMM_WORLD);
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
} /* refineGrid_run */

extern void
refineGrid_del(refineGrid_t *te)
{
	assert(te != NULL);
	assert(*te != NULL);

	refineGridSetup_del(&((*te)->setup));
	gridRegular_del(&((*te)->gridIn));
	gridRegular_del(&((*te)->gridOut));
	gridWriter_del(&((*te)->writer));
	gridReader_del(&((*te)->reader));
	if((*te)->gridIn2 != NULL)
	    gridRegular_del(&((*te)->gridIn));
	if((*te)->reader2 != NULL)
	    gridReader_del(&((*te)->reader2));
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
              const char     *name,
              const char	 *varName)
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

	var = dataVar_new(varName, DATAVARTYPE_FPV, 1);
	gridRegular_attachVar(grid, var);

	return grid;
}

static void
local_fillInputGrid(gridRegular_t grid, gridReader_t reader)
{
	gridPatch_t patch;

	patch = gridRegular_getPatchHandle(grid, 0);

	gridReader_readIntoPatchForVar(reader, patch, 0);
	
}

static void
local_fillOutputGrid(gridRegular_t       gridOut,
                     const gridRegular_t gridIn,
                     const gridRegular_t gridIn2)
{
	gridPatch_t       patchIn, patchOut, patchIn2;
	fpv_t             *dataIn, *dataOut, *dataIn2;
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
		local_enforceConstraints(dataOut, dataIn, dimsOut, dimsIn);
	} else {
		fprintf(stdout, "Interpolation: doing NGP");
		local_dataCopy(dataOut, dataIn, dimsOut, dimsIn);
	}
	
	if (gridIn2 != NULL) {
		patchIn2 = gridRegular_getPatchHandle(gridIn2, 0);
		dataIn2  = (fpv_t *)gridPatch_getVarDataHandle(patchIn2, 0);
		local_addGrid(dataOut, dataIn2, dimsOut);
	}
	
}

static void
local_dataCopy(fpv_t            *dataOut,
              const fpv_t       *dataIn,
              gridPointUint32_t dimsOut,
              gridPointUint32_t dimsIn)
{
#if (NDIM > 2)
#  ifdef WITH_OPENMP
#    pragma omp parallel for
#  endif
	for (uint64_t k = 0; k < dimsOut[2]; k++)
#endif
	{
		int factor_k = dimsIn[2]/dimsOut[2];
		for (uint64_t j = 0; j < dimsOut[1]; j++) {
			int factor_j = dimsIn[1]/dimsOut[1];
			for (uint64_t i = 0; i < dimsOut[0]; i++) {
				int factor_i = dimsIn[0]/dimsOut[0];
				uint64_t    idxOut = i + (j + k * dimsOut[1]) * dimsOut[0];
				uint64_t    idxIn = i*factor_i + (j*factor_j + k*factor_k * dimsIn[1]) * dimsIn[0];
				dataOut[idxOut] = dataIn[idxIn];
			}
		}
	}
}


static void
local_addGrid(fpv_t             *dataOut,
              const fpv_t       *dataIn,
              gridPointUint32_t dimsOut)
{

#if (NDIM > 2)
#  ifdef WITH_OPENMP
#    pragma omp parallel for
#  endif
	for (uint64_t k = 0; k < dimsOut[2]; k++)
#endif
	{
		for (uint64_t j = 0; j < dimsOut[1]; j++) {
			for (uint64_t i = 0; i < dimsOut[0]; i++) {
				uint64_t    idxOut = i + (j + k * dimsOut[1]) * dimsOut[0];
				dataOut[idxOut] += dataIn[idxOut];
			}
		}
	}
} /* local_addGrid */

#define WRAP(i,dim) ((i)<0? (dim)+(i) : ((i)>=(dim)? (i)-(dim) : (i) ))
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
	}

#if (NDIM > 2)
#  ifdef WITH_OPENMP
#    pragma omp parallel for
#  endif
	for (uint64_t k = 0; k < dimsIn[2]; k++)
#endif
	{
		for (uint64_t j = 0; j < dimsIn[1]; j++) {
			for (uint64_t i = 0; i < dimsIn[0]; i++) {
				double   dataInArr[27];
				for (int kk=0;kk<3;kk++)
				  for (int jj=0;jj<3;jj++)
				    for (int ii=0;ii<3;ii++) {
						uint64_t    idxIn = WRAP(i+ii-1,dimsIn[0]) + (WRAP(j+jj-1,dimsIn[1]) + WRAP(k+kk-1,dimsIn[2]) * dimsIn[1]) * dimsIn[0];
						dataInArr[ii+(jj+kk*3)*3] = dataIn[idxIn];
					}
				      

				uint64_t    idxOut = i * dimsSV[0]
				                     + (j * dimsSV[1]
				                        + k * dimsSV[2]
				                        * dimsOut[1]) * dimsOut[0];
				local_CICToSV(dataOut + idxOut, dimsOut, dimsSV,
				              dataInArr);
			}
		}
	}
} /* local_enforceConstraints */

inline static void
local_setToSV(fpv_t             *data,
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
				data[i + (j + k * dimsIn[1]) * dimsIn[0]] = (fpv_t)value;
			}
		}
	}
}

inline static void
local_CICToSV(fpv_t             *data,
              gridPointUint32_t dimsIn,
              gridPointUint32_t dimsSV,
              double 			*valArr)
{
	double    tx[3],ty[3],tz[3];
#if (NDIM > 2)
	for (uint64_t k = 0; k < dimsSV[2]; k++)
#endif
	{
		double dz = ((double)k)/((double) dimsSV[2]) + 1/(2*(double) dimsSV[2]);
		tz[0]=fmax(0, 0.5- dz);
		tz[2]=fmax(0, dz-0.5 );
		tz[1]=fmin( 0.5+dz , 1.5-dz );
		for (uint64_t j = 0; j < dimsSV[1]; j++) {
			double dy = ((double)j)/((double) dimsSV[1]) + 1/(2*(double) dimsSV[1]);
			ty[0]=fmax(0, 0.5-dy );
			ty[2]=fmax(0, dy-0.5 );
			ty[1]=fmin( 0.5+dy , 1.5-dy );
			for (uint64_t i = 0; i < dimsSV[0]; i++) {
				double dx = ((double)i)/((double) dimsSV[0]) + 1/(2*(double) dimsSV[0]);
				tx[0]=fmax(0, 0.5-dx );
				tx[2]=fmax(0, dx-0.5 );
				tx[1]=fmin( 0.5+dx , 1.5-dx );
				double val=0;
				for (int kk=0;kk<3;kk++)
				  for (int jj=0;jj<3;jj++)
				    for (int ii=0;ii<3;ii++)
				       val = val+valArr[ii+(jj+kk*3)*3]*tx[ii]*ty[jj]*tz[kk];
				data[i + (j + k * dimsIn[1]) * dimsIn[0]] = (fpv_t)val;
			}
		}
	}
}
