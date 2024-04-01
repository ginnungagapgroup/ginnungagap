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
#include "refineGrid_adt.h"


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
              gridPointUint32_t dimsIn,
              gridPointUint32_t gridInDims,
			  gridPointUint32_t gridOutDims);

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
                         gridPointUint32_t dimsIn,
                         const gridRegular_t gridIn);


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
              
static void
local_doFilter(gridRegularFFT_t fft, int cut_kind, uint32_t dim1D);

static void
local_doShiftFFT(gridRegularFFT_t fft, uint32_t          dim1D);

static cosmoPk_t
local_calcPk(gridRegularFFT_t gridFFT,
                      uint32_t         dim1D,
                      double           boxsizeInMpch);

static void
local_reducePk(double *pK, double *k, uint32_t *nums, uint32_t kMaxGrid);

static void
local_getGridStuff(gridRegularFFT_t  gridFFT,
                   uint32_t          dim1D,
                   fpvComplex_t      **data,
                   gridPointUint32_t dimsGrid,
                   gridPointUint32_t dimsPatch,
                   gridPointUint32_t idxLo,
                   gridPointUint32_t kMaxGrid);

static double
local_kernel1D(double x);

static double
local_cutoff(double f, double rs);

static void
local_addGridVal(gridRegular_t       gridOut,
              double val);

/*--- Implementations of exported functios ------------------------------*/
extern refineGrid_t
refineGrid_newFromIni(parse_ini_t ini)
{
	refineGrid_t te;
	assert(ini != NULL);
	int  dim1D_proto;

	te        = xmalloc(sizeof(struct refineGrid_struct));

	te->setup = refineGridSetup_newFromIni(ini,
	                                                 "Setup");
	if(te->setup->outputDim1D > te->setup->inputDim1D) {
		dim1D_proto = te->setup->inputDim1D;
	} else {
		dim1D_proto = te->setup->outputDim1D;
	}
	
	te->gridIn = local_getGrid(te->setup->boxsizeInMpch,
	                           te->setup->inputDim1D,
	                           "Input",
	                           te->setup->varName,
	                           dim1D_proto,
	                           &te->distribIn);
	te->gridOut = local_getGrid(te->setup->boxsizeInMpch,
	                            te->setup->outputDim1D,
	                            "Output",
	                            te->setup->varName,
	                            dim1D_proto,
	                            &te->distribOut);
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
	                           "Input2",
	                           te->setup->varName,
	                           dim1D_proto,
	                           &te->distribIn2);
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
	gridRegularFFT_t fft1, fft2;
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

    if(te->setup->addFields) {
		timing = timer_start_text("  Filling second input grid... ");
		local_fillInputGrid(te->gridIn2, te->reader2);
		timing = timer_stop_text(timing, "took %.5fs\n");
	
		timing = timer_start_text("  Calculating statistics on second input grid... ");
#ifdef WITH_MPI	
		gridStatistics_calcGridRegularDistrib(stat, te->distribIn2, 0);
#else
		gridStatistics_calcGridRegular(stat, te->gridIn2, 0);
#endif
		timing = timer_stop_text(timing, "took %.5fs\n");
		if (rank == 0)
			gridStatistics_printPretty(stat, stdout, "  ");
			
		
		timing = timer_start_text("  Filtering first grid in k-space... ");
		fft1 = gridRegularFFT_new(te->gridIn,te->distribIn,0);
		gridRegularFFT_execute(fft1, GRIDREGULARFFT_FORWARD);
		local_doFilter(fft1,CUT_SMALL,te->setup->inputDim1D);
		gridRegularFFT_execute(fft1, GRIDREGULARFFT_BACKWARD);
		gridRegularFFT_del(&fft1);
                local_addGridVal(te->gridIn,mean);
		timing = timer_stop_text(timing, "took %.5fs\n");
		
		timing = timer_start_text("  Filtering second grid in k-space... ");
		fft2 = gridRegularFFT_new(te->gridIn2,te->distribIn2,0);
		gridRegularFFT_execute(fft2, GRIDREGULARFFT_FORWARD);
		local_doFilter(fft2,CUT_LARGE,te->setup->inputDim1D);
		gridRegularFFT_execute(fft2, GRIDREGULARFFT_BACKWARD);
		gridRegularFFT_del(&fft2);
		timing = timer_stop_text(timing, "took %.5fs\n");
	}
	
	if(te->setup->inputDim1D > te->setup->outputDim1D) {
		timing = timer_start_text("  FFT correction before NGP interpolation... ");
		fft1 = gridRegularFFT_new(te->gridIn,te->distribIn,0);
		gridRegularFFT_execute(fft1, GRIDREGULARFFT_FORWARD);
		local_doShiftFFT(fft1,te->setup->inputDim1D);
		gridRegularFFT_execute(fft1, GRIDREGULARFFT_BACKWARD);
		gridRegularFFT_del(&fft1);
		timing = timer_stop_text(timing, "took %.5fs\n");
	}

	timing = timer_start_text("  Filling output grid... ");
	local_fillOutputGrid(te->gridOut, te->gridIn, te->gridIn2);
	timing = timer_stop_text(timing, "took %.5fs\n");

	timing = timer_start_text("  Calculating statistics on output grid... ");
#ifdef WITH_MPI 
        gridStatistics_calcGridRegularDistrib(stat, te->distribOut, 0);
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

	if(te->setup->doPk) {
		gridRegularFFT_t fft = gridRegularFFT_new(te->gridOut,te->distribOut,0);
		cosmoPk_t pk;
		timing = timer_start_text("  Calculating P(k)...");
		gridRegularFFT_execute(fft, GRIDREGULARFFT_FORWARD);
		pk     = local_calcPk(fft,
				                 te->setup->outputDim1D,
				                               te->setup->boxsizeInMpch);
				cosmoPk_dumpToFile(pk, te->setup->PkFile, 1);
				cosmoPk_del(&pk);
		timing = timer_stop_text(timing, "took %.5fs\n");
	}

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
	    gridRegular_del(&((*te)->gridIn2));
	if((*te)->reader2 != NULL)
	    gridReader_del(&((*te)->reader2));
	xfree(*te);

	*te = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_getGridStuff(gridRegularFFT_t  gridFFT,
                   uint32_t          dim1D,
                   fpvComplex_t      **data,
                   gridPointUint32_t dimsGrid,
                   gridPointUint32_t dimsPatch,
                   gridPointUint32_t idxLo,
                   gridPointUint32_t kMaxGrid)
{
	gridRegular_t grid;
	gridPatch_t   patch;

	grid = gridRegularFFT_getGridFFTed(gridFFT);
	gridRegular_getDims(grid, dimsGrid);
	patch = gridRegular_getPatchHandle(grid, 0);
	gridPatch_getDims(patch, dimsPatch);
	gridPatch_getIdxLo(patch, idxLo);
	*data = gridPatch_getVarDataHandle(patch, 0);

	for (int i = 0; i < NDIM; i++)
		kMaxGrid[i] = dim1D / 2;
	kMaxGrid[gridRegular_getCurrentDim(grid, 0)]++; // This is the r2c FFT
	                                                // dimension
}

static double
local_kernel1D(double x)
{
	double t;
	if (x==0)
		t = 1.0;
	else
		t = sin(x)/x;
	return t*t;
} /* local_kernel1D */

static double
local_cutoff(double f, double rs)
{
	return (f*rs>1)? 0.0 : 1.0;
}

#define WRAP_WAVENUM(k, kmax, dims) \
    k = (k > kmax) ? k - dims : k

#define WAVE_SQR(k) \
    (k[0] * k[0] + k[1] * k[1] + k[2] * k[2])

static void
local_doFilter(gridRegularFFT_t fft, int cut_kind, uint32_t          dim1D)
{
	double norm          = (gridRegularFFT_getNorm(fft));
	gridPointUint32_t dimsGrid, dimsPatch, idxLo, kMaxGrid;
	fpvComplex_t      *data;

	local_getGridStuff(fft, dim1D, &data, dimsGrid, dimsPatch,
	                   idxLo, kMaxGrid);
	                   
	const uint32_t realGrid = dimsGrid[0]>dimsGrid[1] ? dimsGrid[0] : dimsGrid[1]; // because one of them is r2c dimension
	
	double rsSqr = (4.0/dim1D)*(4.0/dim1D);

	//printf("%i %i\n",);

#ifdef _OPENMP
#  pragma omp parallel for shared(dimsPatch, idxLo, kMaxGrid, \
	dimsGrid, data)
#endif
	for (uint64_t k = 0; k < dimsPatch[2]; k++) {
		int64_t kReal[3];
		kReal[2] = k + idxLo[2];
		WRAP_WAVENUM(kReal[2], kMaxGrid[2], dimsGrid[2]);
		for (uint64_t j = 0; j < dimsPatch[1]; j++) {
			kReal[1] = j + idxLo[1];
			WRAP_WAVENUM(kReal[1], kMaxGrid[1], dimsGrid[1]);
			for (uint64_t i = 0; i < dimsPatch[0]; i++) {
				double   kCellSqr;
				uint64_t idx;

				kReal[0] = i + idxLo[0];
				WRAP_WAVENUM(kReal[0], kMaxGrid[0], dimsGrid[0]);

				idx      = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCellSqr = ((double)(WAVE_SQR(kReal)));
				

				if ((kReal[0] == 0) && (kReal[1] == 0)
				    && (kReal[2] == 0)) {
					data[idx] = 0.0;
				} else {
					data[idx] *= (fpv_t)(norm);
					if (cut_kind == CUT_SMALL) {
						data[idx] *= (fpv_t)local_cutoff(kCellSqr, rsSqr);
                        //                        data[idx] *= exp(-kCellSqr*rsSqr/2);
						if(kReal[0]!=0)
							data[idx] /= (fpv_t)local_kernel1D(((double)kReal[0])*M_PI/realGrid);
						if(kReal[1]!=0)
							data[idx] /= (fpv_t)local_kernel1D(((double)kReal[1])*M_PI/realGrid);
						if(kReal[2]!=0)
							data[idx] /= (fpv_t)local_kernel1D(((double)kReal[2])*M_PI/realGrid);
					} else
						data[idx] *= (fpv_t)(1 - local_cutoff(kCellSqr, rsSqr));
                        //                        data[idx] *= 1-exp(-kCellSqr*rsSqr/2);
				}
			}
		}
	}
}


static void
local_doShiftFFT(gridRegularFFT_t fft, uint32_t          dim1D)
{
	double norm          = (gridRegularFFT_getNorm(fft));
	gridPointUint32_t dimsGrid, dimsPatch, idxLo, kMaxGrid;
	fpvComplex_t      *data;

	local_getGridStuff(fft, dim1D, &data, dimsGrid, dimsPatch,
	                   idxLo, kMaxGrid);
	                   
	const uint32_t realGrid = dimsGrid[0]>dimsGrid[1] ? dimsGrid[0] : dimsGrid[1]; // because one of them is r2c dimension
	
#ifdef _OPENMP
#  pragma omp parallel for shared(dimsPatch, idxLo, kMaxGrid, \
	dimsGrid, data)
#endif
	for (uint64_t k = 0; k < dimsPatch[2]; k++) {
		int64_t kReal[3];
		kReal[2] = k + idxLo[2];
		WRAP_WAVENUM(kReal[2], kMaxGrid[2], dimsGrid[2]);
		double kz = ((double)kReal[2])/dim1D;
		for (uint64_t j = 0; j < dimsPatch[1]; j++) {
			kReal[1] = j + idxLo[1];
			WRAP_WAVENUM(kReal[1], kMaxGrid[1], dimsGrid[1]);
			double ky = ((double)kReal[1])/dim1D;
			for (uint64_t i = 0; i < dimsPatch[0]; i++) {
				double   kCellSqr;
				uint64_t idx;

				kReal[0] = i + idxLo[0];
				WRAP_WAVENUM(kReal[0], kMaxGrid[0], dimsGrid[0]);
				double kx = ((double)kReal[0])/dim1D;

				idx      = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCellSqr = ((double)(WAVE_SQR(kReal)));
				

				if ((kReal[0] == 0) && (kReal[1] == 0)
				    && (kReal[2] == 0)) {
					data[idx] = 0.0;
				} else {
					data[idx] *= (fpv_t)(norm);
					data[idx] *= cos(M_PI*(kx+ky+kz))+I*sin(M_PI*(kx+ky+kz));
					if(abs(kx)>0.25 || abs(ky)>0.25 || abs(kz)>0.25)
							data[idx] = FPV_C(0.0);
				}
			}
		}
	}
}



static cosmoPk_t
local_calcPk(gridRegularFFT_t gridFFT,
                      uint32_t         dim1D,
                      double           boxsizeInMpch)
{
	cosmoPk_t         pk;
	gridPointUint32_t dimsGrid, dimsPatch, idxLo, kMaxGrid;
	fpvComplex_t      *data;
	double            wavenumToFreq, *P, *freq, volume;
	uint32_t          *numFreqHits;

	assert(gridFFT != NULL);

	double norm          = (gridRegularFFT_getNorm(gridFFT));

	local_getGridStuff(gridFFT, dim1D, &data, dimsGrid, dimsPatch,
	                   idxLo, kMaxGrid);
	wavenumToFreq = 2. * M_PI * 1. / boxsizeInMpch;
	volume        = boxsizeInMpch * boxsizeInMpch * boxsizeInMpch;
	P             = xmalloc(sizeof(double) * kMaxGrid[0]);
	freq          = xmalloc(sizeof(double) * kMaxGrid[0]);
	numFreqHits   = xmalloc(sizeof(uint32_t) * kMaxGrid[0]);
	for (int i = 0; i < kMaxGrid[0]; i++) {
		P[i]           = 0.0;
		freq[i]        = -1.0;
		numFreqHits[i] = 0;
	}

	for (uint64_t k = 0; k < dimsPatch[2]; k++) {
		int64_t k2 = k + idxLo[2];
		k2 = (k2 > kMaxGrid[2]) ? k2 - dimsGrid[2] : k2;
		for (uint64_t j = 0; j < dimsPatch[1]; j++) {
			int64_t k1 = j + idxLo[1];
			k1 = (k1 > kMaxGrid[1]) ? k1 - dimsGrid[1] : k1;
			for (uint64_t i = 0; i < dimsPatch[0]; i++) {
				int64_t  k0 = i + idxLo[0];
				uint64_t idx;
				int      kCell;

				k0    = (k0 > kMaxGrid[0]) ? k0 - dimsGrid[0] : k0;
				idx   = i + (j + k * dimsPatch[1]) * dimsPatch[0];
				kCell = (int)floor(sqrt((double)(k0 * k0 + k1 * k1
				                                 + k2 * k2)));
				data[idx] *= norm;

				if ((kCell <= kMaxGrid[0]) && (kCell > 0)) {
					P[kCell - 1]   += creal(data[idx]) * creal(data[idx])
					                  + cimag(data[idx]) * cimag(data[idx]);
					freq[kCell - 1] = kCell * wavenumToFreq;
					numFreqHits[kCell - 1]++;
				}
			}
		}
	}
#ifdef WITH_MPI
	local_reducePk(P, freq, numFreqHits, kMaxGrid[0]);
#endif
	for (uint32_t i = 0; i < kMaxGrid[0]; i++)
		P[i] *= (volume / numFreqHits[i]);

	pk = cosmoPk_newFromArrays(kMaxGrid[0], freq, P,
	                           (P[5] - P[0]) / (freq[5] - freq[0]),
	                           (P[kMaxGrid[0] - 1] - P[kMaxGrid[0] - 6])
	                           / (freq[kMaxGrid[0] - 1]
	                              - freq[kMaxGrid[0] - 6]));

	xfree(numFreqHits);
	xfree(freq);
	xfree(P);

	return pk;
} /* ginnungagapIC_calcPowerSpectrum */

#ifdef WITH_MPI
static void
local_reducePk(double *pK, double *k, uint32_t *nums, uint32_t kMaxGrid)
{
	double   *tmp;
	uint32_t *tmpInt;

	tmp = xmalloc(sizeof(double) * kMaxGrid);
	memcpy(tmp, pK, kMaxGrid * sizeof(double));
	MPI_Allreduce(tmp, pK, (int)kMaxGrid, MPI_DOUBLE, MPI_SUM,
	              MPI_COMM_WORLD);
	memcpy(tmp, k, kMaxGrid * sizeof(double));
	MPI_Allreduce(tmp, k, (int)kMaxGrid, MPI_DOUBLE, MPI_MAX,
	              MPI_COMM_WORLD);
	xfree(tmp);

	tmpInt = xmalloc(sizeof(uint32_t) * kMaxGrid);
	memcpy(tmpInt, nums, kMaxGrid * sizeof(uint32_t));
	MPI_Allreduce(tmpInt, nums, (int)kMaxGrid, MPI_UNSIGNED,
	              MPI_SUM, MPI_COMM_WORLD);
	xfree(tmpInt);
}

#endif

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
local_fillOutputGrid(gridRegular_t       gridOut,
                     const gridRegular_t gridIn,
                     const gridRegular_t gridIn2)
{
	gridPatch_t       patchIn, patchOut, patchIn2;
	fpv_t             *dataIn, *dataOut, *dataIn2;
	gridPointUint32_t dimsIn, dimsOut;
	gridPointUint32_t factors, gridInDims, gridOutDims;

	patchIn = gridRegular_getPatchHandle(gridIn, 0);
	dataIn  = (fpv_t *)gridPatch_getVarDataHandle(patchIn, 0);
	gridPatch_getDims(patchIn, dimsIn);

	patchOut = gridRegular_getPatchHandle(gridOut, 0);
	dataOut  = (fpv_t *)gridPatch_getVarDataHandle(patchOut, 0);
	gridPatch_getDims(patchOut, dimsOut);

	gridRegular_getDims(gridIn, gridInDims);
	gridRegular_getDims(gridOut, gridOutDims);
	//for (int k = 0; k<NDIM; k++)
	//		factors[k] = gridInDims[k]/gridOutDims[k];

	if ((dimsIn[0] < dimsOut[0]) && (dimsIn[1] < dimsOut[1])
	    && (dimsIn[2] < dimsOut[2])) {
		local_enforceConstraints(dataOut, dataIn, dimsOut, dimsIn, gridIn);
	} else {
		fprintf(stdout, " (Interpolation: doing NGP)\n");
		local_dataCopy(dataOut, dataIn, dimsOut, dimsIn, gridInDims,gridOutDims);
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
              gridPointUint32_t dimsIn,
              gridPointUint32_t gridInDims,
			  gridPointUint32_t gridOutDims)
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
				uint64_t    idxIn = i*gridInDims[0]/gridOutDims[0] + (j*gridInDims[1]/gridOutDims[1] + k*gridInDims[2]/gridOutDims[2] * dimsIn[1]) * dimsIn[0];
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

static void
local_addGridVal(gridRegular_t       gridOut,
              double val)
{
gridPointUint32_t dimsOut;
gridPatch_t patchOut;
fpv_t             *dataOut;
patchOut = gridRegular_getPatchHandle(gridOut, 0);
dataOut  = (fpv_t *)gridPatch_getVarDataHandle(patchOut, 0);
gridPatch_getDims(patchOut, dimsOut);

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
				dataOut[idxOut] += (fpv_t) val;
			}
		}
	}
} /* local_addGridVal */

#define WRAP(i,dim) ((i)<0? (dim)+(i) : ((i)>=(dim)? (i)-(dim) : (i) ))
static void
local_enforceConstraints(fpv_t             *dataOut,
                         const fpv_t       *dataIn,
                         gridPointUint32_t dimsOut,
                         gridPointUint32_t dimsIn,
                         const gridRegular_t gridIn)
{
	gridPointUint32_t dimsSV;
	uint64_t bufsize = dimsIn[0]*dimsIn[1];

	fpv_t *buffSendLo = malloc(sizeof(fpv_t) * bufsize);
	fpv_t *buffSendHi = malloc(sizeof(fpv_t) * bufsize);
	fpv_t *buffLo = malloc(sizeof(fpv_t) * bufsize);
	fpv_t *buffHi = malloc(sizeof(fpv_t) * bufsize);
	
	for(uint64_t j = 0; j < dimsIn[1]; j++) {
		for(uint64_t i = 0; i < dimsIn[0]; i++) {
			uint64_t idx = i + j*dimsIn[0];
			buffLo[idx] = dataIn[i+(j+(dimsIn[2]-1)*dimsIn[1])*dimsIn[0]];
			buffHi[idx] = dataIn[idx];
			buffSendLo[idx] = buffLo[idx];
			buffSendHi[idx] = buffHi[idx];
		}
	}

#ifdef WITH_MPI
    int tile     = 0;
	int numTiles = 1;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &numTiles);
	MPI_Comm_rank(MPI_COMM_WORLD, &tile);
	if(numTiles>1) {
		int tile_lo = WRAP(tile-1,numTiles);
		int tile_hi = WRAP(tile+1,numTiles);
		if(tile%2 == 0) {
			MPI_Send(&(buffSendLo[0]), bufsize, MPI_FLOAT, tile_hi, 1, MPI_COMM_WORLD);
			MPI_Send(&(buffSendHi[0]), bufsize, MPI_FLOAT, tile_lo, 2, MPI_COMM_WORLD);
			MPI_Recv(&(buffLo[0]), bufsize, MPI_FLOAT, tile_lo, 3, MPI_COMM_WORLD, &status);
			MPI_Recv(&(buffHi[0]), bufsize, MPI_FLOAT, tile_hi, 4, MPI_COMM_WORLD, &status);
		} else {
			MPI_Recv(&(buffLo[0]), bufsize, MPI_FLOAT, tile_lo, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(&(buffHi[0]), bufsize, MPI_FLOAT, tile_hi, 2, MPI_COMM_WORLD, &status);
			MPI_Send(&(buffSendLo[0]), bufsize, MPI_FLOAT, tile_hi, 3, MPI_COMM_WORLD);
			MPI_Send(&(buffSendHi[0]), bufsize, MPI_FLOAT, tile_lo, 4, MPI_COMM_WORLD);
		}
	}
	
#endif

	for (int i = 0; i < NDIM; i++) {
		assert(dimsOut[i] % dimsIn[i] == 0);
		dimsSV[i]           = dimsOut[i] / dimsIn[i];
	}

#if (NDIM > 2)
#  ifdef WITH_OPENMP
#    pragma omp parallel for
#  endif
	for (int64_t k = 0; k < dimsIn[2]; k++)
#endif
	{
		for (int64_t j = 0; j < dimsIn[1]; j++) {
			for (int64_t i = 0; i < dimsIn[0]; i++) {
				double   dataInArr[27];
				for (int kk=0;kk<3;kk++)
				  for (int jj=0;jj<3;jj++)
				    for (int ii=0;ii<3;ii++) {
						if(k+kk-1>=0 && k+kk-1<dimsIn[2]) {
							uint64_t    idxIn = WRAP(i+ii-1,dimsIn[0]) + (WRAP(j+jj-1,dimsIn[1]) + WRAP(k+kk-1,dimsIn[2]) * dimsIn[1]) * dimsIn[0];
							dataInArr[ii+(jj+kk*3)*3] = dataIn[idxIn];
						} else {
							uint64_t    idxIn = WRAP(i+ii-1,dimsIn[0]) + WRAP(j+jj-1,dimsIn[1]) * dimsIn[0];
							if(k+kk-1<0)
								dataInArr[ii+(jj+kk*3)*3] = buffLo[idxIn];
							if(k+kk-1>=dimsIn[2])
								dataInArr[ii+(jj+kk*3)*3] = buffHi[idxIn];
						}
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
	
	free(buffSendLo);
	free(buffSendHi);
	free(buffLo);
	free(buffHi);
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
