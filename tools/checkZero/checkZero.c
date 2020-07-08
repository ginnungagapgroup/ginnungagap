// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file checkZero/checkZero.c
 * @ingroup  toolscheckZero
 * @brief  Provides the implementation of the checkZero tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "checkZeroConfig.h"
#include "checkZero.h"
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#ifdef WITH_OPENMP
#  include <omp.h>
#endif
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"
#include "../../src/libgrid/gridRegularFFT.h"
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
#include "../../src/libutil/utilMath.h"
#include "../../src/libcosmo/cosmoPk.h"
#include "../../src/libcosmo/cosmoModel.h"
#ifdef WITH_FFT_FFTW3
#  include <complex.h>
#  include <fftw3.h>
#endif

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define NBR(x,dx,dim) ( MIN(MAX(((x)+(dx)-1),0),((dim)-1) ) )
#define CUT_SMALL 0
#define CUT_LARGE 1


/*--- Implemention of main structure ------------------------------------*/
#include "checkZero_adt.h"


/*--- Prototypes of local functions -------------------------------------*/

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
              const char     *name,
              const char	 *varName,
              int            dim1D_proto,
              gridRegularDistrib_t *distrib);


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
 * @brief  Compute the number of zeros in the grid.
 *
 * @param[in]   grid
 *                 The input grid.
 * 
 * @return  Returns nothing.
 */
static void
local_searchZeros(const gridRegular_t gridIn);

/*--- Implementations of exported functios ------------------------------*/
extern checkZero_t
checkZero_newFromIni(parse_ini_t ini)
{
	checkZero_t te;
	assert(ini != NULL);
	int  dim1D_proto;

	te        = xmalloc(sizeof(struct checkZero_struct));

	te->setup = checkZeroSetup_newFromIni(ini,
	                                                 "Setup");
    dim1D_proto = te->setup->inputDim1D;
	
	
	te->gridIn = local_getGrid(te->setup->boxsizeInMpch,
	                           te->setup->inputDim1D,
	                           "Input",
	                           te->setup->varName,
	                           dim1D_proto,
	                           &te->distribIn);
	te->reader   = gridReaderFactory_newReaderFromIni(
		    ini, te->setup->readerSecName);
	
	return te;
} /* checkZero_newFromIni */

extern void
checkZero_run(checkZero_t te)
{
	double           timing;
	gridStatistics_t stat;
        double   mean;
	int              rank = 0, size = 1;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif

	assert(te != NULL);
	
	stat   = gridStatistics_new();

	timing = timer_start_text("  Filling input grid... ");
	local_fillInputGrid(te->gridIn, te->reader);
	timing = timer_stop_text(timing, "took %.5fs\n");

	timing = timer_start_text("  Calculating statistics on input grid... ");
#ifdef WITH_MPI	
	gridStatistics_calcGridRegularDistrib(stat, te->distribIn, 0);
#else
	gridStatistics_calcGridRegular(stat, te->gridIn, 0);
#endif
        mean = gridStatistics_getMean(stat);
	timing = timer_stop_text(timing, "took %.5fs\n");
	if (rank == 0)
		gridStatistics_printPretty(stat, stdout, "  ");

    
	timing = timer_start_text("  Search for zeros in data... ");
	local_searchZeros(te->gridIn);
	timing = timer_stop_text(timing, "took %.5fs\n");

	

	gridStatistics_del(&stat);
} /* checkZero_run */

extern void
checkZero_del(checkZero_t *te)
{
	assert(te != NULL);
	assert(*te != NULL);

	checkZeroSetup_del(&((*te)->setup));
	gridRegular_del(&((*te)->gridIn));
	gridReader_del(&((*te)->reader));
	xfree(*te);

	*te = NULL;
}


static gridRegular_t
local_getGrid(double         boxsizeInMpch,
              uint32_t       dim1D,
              const char     *name,
              const char	 *varName,
              int            dim1D_proto,
              gridRegularDistrib_t *distrib)
{
	gridPointDbl_t    origin, extent;
	gridPointUint32_t dims;
	gridRegular_t     grid;
	gridPatch_t       patch;
	dataVar_t         var;
	gridPointInt_t nProc;

	nProc[0] = 1;
	nProc[1] = 1;
	nProc[2] = 0;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = (double)(boxsizeInMpch);
		dims[i]   = dim1D;
	}
	grid  = gridRegular_new(name, origin, extent, dims);
	
	*distrib = gridRegularDistrib_new(grid, NULL);
#ifdef WITH_MPI	
	gridRegularDistrib_initMPI(*distrib, nProc, MPI_COMM_WORLD);
#endif
    int rank = gridRegularDistrib_getLocalRank(*distrib);
    gridRegularDistrib_setFactorFromDim(*distrib, dim1D, dim1D_proto);
    int a,b;
    gridRegularDistrib_getFactor(*distrib, &a,&b);
	patch = gridRegularDistrib_getPatchForRank(*distrib, rank);
	gridRegular_attachPatch(grid, patch);
	gridPointUint32_t idxLo, dimspatch;
	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dimspatch);
	

	var = dataVar_new(varName, DATAVARTYPE_FPV, 1);
#ifdef WITH_FFT_FFTW3
#  ifdef ENABLE_DOUBLE
	dataVar_setMemFuncs(var, &fftw_malloc, &fftw_free);
#  else
	dataVar_setMemFuncs(var, &fftwf_malloc, &fftwf_free);
#  endif
#endif
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
local_searchZeros(const gridRegular_t gridIn)
{
	gridPatch_t       patchIn;
	fpv_t             *dataIn;
	gridPointUint32_t dimsIn;
    unsigned long int nzeros = 0, nzeros_global = 0;
	int              rank = 0, size = 1;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif

	patchIn = gridRegular_getPatchHandle(gridIn, 0);
	dataIn  = (fpv_t *)gridPatch_getVarDataHandle(patchIn, 0);
	gridPatch_getDims(patchIn, dimsIn);

    
#if (NDIM > 2)
#  ifdef WITH_OPENMP
#    pragma omp parallel for
#  endif
	for (uint64_t k = 0; k < dimsIn[2]; k++)
#endif
	{
		for (uint64_t j = 0; j < dimsIn[1]; j++) {
			for (uint64_t i = 0; i < dimsIn[0]; i++) {
				uint64_t    idx = i + (j + k * dimsIn[1]) * dimsIn[0];
                if (dataIn[idx]==0.0) nzeros++;
            }
		}
	}
#ifdef WITH_MPI	    
    MPI_Reduce(&nzeros, &nzeros_global, 1, MPI_UNSIGNED_LONG, MPI_SUM, 0,
           MPI_COMM_WORLD);
#else
    nzeros_global = nzeros;
#endif

    if (rank == 0) {
        printf("Zeros in grid: %lu\n", nzeros_global);
    }
	
}

