// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegularFFT_tests.h"
#include "gridRegularFFT.h"
#include "gridRegular.h"
#include "gridRegularDistrib.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef ENABLE_FFT_BACKEND_FFTW3
#  include <fftw3.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularFFT_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getFakeGrid(void);

static gridRegularDistrib_t
local_getFakeGridDistrib(gridRegular_t grid);

static void
local_fillFakeGrid(gridRegular_t grid);

static bool
local_testFFTResult(gridRegular_t grid, fpv_t *dataCpy);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridRegularFFT_new_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularFFT_t     fft;
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid    = local_getFakeGrid();
	distrib = local_getFakeGridDistrib(grid);
	fft     = gridRegularFFT_new(grid, distrib, 0);
	if (gridVar_getType(fft->var) != GRIDVARTYPE_FPV)
		hasPassed = false;
	gridRegular_del(&grid);
	gridRegularDistrib_del(&distrib);
	gridRegularFFT_del(&fft);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegularFFT_new_test */

extern bool
gridRegularFFT_del_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularFFT_t     fft;
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid    = local_getFakeGrid();
	distrib = local_getFakeGridDistrib(grid);
	fft     = gridRegularFFT_new(grid, distrib, 0);
	gridRegular_del(&grid);
	gridRegularDistrib_del(&distrib);
	gridRegularFFT_del(&fft);
	if (fft != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegularFFT_execute_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularFFT_t     fft;
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
	gridPatch_t          patch;
	fpvComplex_t         *data;
	fpv_t                *dataBack;
	fpv_t                *dataCpy, *dataTmp;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid    = local_getFakeGrid();
	distrib = local_getFakeGridDistrib(grid);
	local_fillFakeGrid(grid);
	patch   = gridRegular_getPatchHandle(grid, 0);
	dataTmp = gridPatch_getVarDataHandle(patch, 0);
	dataCpy = xmalloc(sizeof(fpv_t)
	                  * gridPatch_getNumCellsActual(patch, 0));
	memcpy(dataCpy, dataTmp,
	       sizeof(fpv_t) * gridPatch_getNumCellsActual(patch, 0));
	fft      = gridRegularFFT_new(grid, distrib, 0);
	data     = (fpvComplex_t *)gridRegularFFT_execute(fft, 1);
	dataBack = (fpv_t *)gridRegularFFT_execute(fft, -1);
	if ((void *)data != (void *)dataBack)
		hasPassed = false;
	if (!local_testFFTResult(grid, dataCpy))
		hasPassed = false;
	gridRegular_del(&grid);
	gridRegularDistrib_del(&distrib);
	gridRegularFFT_del(&fft);
	xfree(dataCpy);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegularFFT_execute_test */

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getFakeGrid(void)
{
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridVar_t         var;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = 1.0;
		dims[i]   = 32 + i;
	}
	var = gridVar_new("test", GRIDVARTYPE_FPV, 1);
#ifndef WITH_MPI
	gridVar_setFFTWPadded(var);
#endif
#ifdef ENABLE_FFT_BACKEND_FFTW3
#  ifdef ENABLE_DOUBLE
	gridVar_setMemFuncs(var, &fftw_malloc, &fftw_free);
#  else
	gridVar_setMemFuncs(var, &fftwf_malloc, &fftwf_free);
#  endif
#endif

	grid = gridRegular_new("bla", origin, extent, dims);
	gridRegular_attachVar(grid, var);

	return grid;
}

static gridRegularDistrib_t
local_getFakeGridDistrib(gridRegular_t grid)
{
	gridRegularDistrib_t distrib;
	int                  rank = 0;
#ifdef WITH_MPI
	gridPointInt_t       nProcs;
#endif
	gridPatch_t          patch;

	distrib = gridRegularDistrib_new(grid, NULL);
#ifdef WITH_MPI
	for (int i = 0; i < NDIM - 1; i++)
		nProcs[i] = 1;
	MPI_Comm_size(MPI_COMM_WORLD, &(nProcs[NDIM - 1]));
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
	rank = gridRegularDistrib_getLocalRank(distrib);
#endif

	patch = gridRegularDistrib_getPatchForRank(distrib, rank);
	gridRegular_attachPatch(grid, patch);

	return distrib;
}

#define PI 3.14159265359
static void
local_fillFakeGrid(gridRegular_t grid)
{
	gridPointUint32_t dims;
	gridPointUint32_t dimsActual;
	uint64_t          offset = UINT64_C(0);
	gridPatch_t       patch  = gridRegular_getPatchHandle(grid, 0);
	fpv_t             *data  = gridPatch_getVarDataHandle(patch, 0);

	gridPatch_getDims(patch, dims);
	gridPatch_getDimsActual(patch, 0, dimsActual);

#if (NDIM == 3)
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			offset = dimsActual[0] * (j + k * dimsActual[1]);
			for (int i = 0; i < dims[0]; i++) {
				data[offset++] = (fpv_t)
				                 (sin(4 * i / ((double)(dims[0])) * PI)
				                  * sin(4 * j / ((double)(dims[1])) * PI)
				                  * sin(4 * k / ((double)(dims[2])) * PI));
			}
		}
	}
#elif (NDIM == 2)
	for (int j = 0; j < dims[1]; j++) {
		offset = dimsActual[0] * (j + k * dimsActual[1]);
		for (int i = 0; i < dims[0]; i++) {
			data[offset++] = (fpv_t)
			                 (sin(4 * i / ((double)(dims[0])) * PI)
			                  * sin(4 * j / ((double)(dims[1])) * PI));
		}
	}
#endif
} /* local_fillFakeGrid */

static bool
local_testFFTResult(gridRegular_t grid, fpv_t *dataCpy)
{
	gridPointUint32_t dims;
	gridPointUint32_t dimsActual;
	uint64_t          normFac = 1;
	uint64_t          offset  = UINT64_C(0);
	gridPatch_t       patch   = gridRegular_getPatchHandle(grid, 0);
	fpv_t             *data   = gridPatch_getVarDataHandle(patch, 0);
	long double       sumSqr  = 0.;
	gridVar_t         var     = gridPatch_getVarHandle(patch, 0);
	gridVarType_t     varType = gridVar_getType(var);

	gridPatch_getDims(patch, dims);
	gridPatch_getDimsActual(patch, 0, dimsActual);

	for (int i = 0; i < NDIM; i++)
		normFac *= dims[i];

#if (NDIM == 3)
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			offset = dimsActual[0] * (j + k * dimsActual[1]);
			for (int i = 0; i < dims[0]; i++) {
				double tmp = data[offset] - dataCpy[offset] * normFac;
				sumSqr += tmp * tmp;
				offset++;
			}
		}
	}
#elif (NDIM == 2)
	for (int j = 0; j < dims[1]; j++) {
		offset = dimsActual[0] * (j + k * dimsActual[1]);
		for (int i = 0; i < dims[0]; i++) {
			double tmp = data[offset] - dataCpy[offset] * normFac;
			sumSqr += tmp * tmp;
			offset++;
		}
	}
#endif

	if (gridVarType_isNativeDouble(varType)) {
		if ((sqrtl(sumSqr) > 2e-9))
			return false;
	} else {
		if ((sqrtl(sumSqr) > 7e-1))
			return false;
	}

	return true;
} /* local_testFFTResult */
