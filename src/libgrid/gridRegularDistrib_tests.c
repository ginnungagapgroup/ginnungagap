// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegularDistrib_tests.h"
#include "gridRegularDistrib.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_SILO
#  include "gridWriterSilo.h"
#  include <silo.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularDistrib_adt.h"
#include "gridPatch_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getFakeGrid(void);


#ifdef WITH_MPI
static void
local_getFakeNProcs(gridPointInt_t nProcs);

#endif

static gridRegularDistrib_t
local_getFakeDistribForTranspose(void);

static void
local_fillFakeGridForTranspose(gridRegular_t grid);

static bool
local_verifyFakeDistribForTranspose(gridRegularDistrib_t distrib);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridRegularDistrib_new_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularDistrib_t distrib;
	gridRegular_t        fakeGrid;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fakeGrid = local_getFakeGrid();
	distrib  = gridRegularDistrib_new(fakeGrid, NULL);
	if (distrib->grid != fakeGrid)
		hasPassed = false;
	if (distrib->numProcs != 1)
		hasPassed = false;
	gridRegular_del(&fakeGrid);
	gridRegularDistrib_del(&distrib);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegularDistrib_del_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularDistrib_t distrib;
	gridRegular_t        fakeGrid;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fakeGrid = local_getFakeGrid();
	distrib  = gridRegularDistrib_new(fakeGrid, NULL);
	gridRegular_del(&fakeGrid);
	gridRegularDistrib_del(&distrib);
	if (distrib != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridRegularDistrib_getRef_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularDistrib_t distrib;
	gridRegularDistrib_t distribBack;
	gridRegular_t        fakeGrid;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fakeGrid    = local_getFakeGrid();
	distrib     = gridRegularDistrib_new(fakeGrid, NULL);
	gridRegular_del(&fakeGrid);
	distribBack = gridRegularDistrib_getRef(distrib);
	if (distribBack != distrib)
		hasPassed = false;
	if (distribBack->refCounter != 2)
		hasPassed = false;
	gridRegularDistrib_del(&distrib);
	if (distribBack->refCounter != 1)
		hasPassed = false;
	gridRegularDistrib_del(&distribBack);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegularDistrib_getRef_test */

#ifdef WITH_MPI
extern bool
gridRegularDistrib_initMPI_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularDistrib_t distrib;
	gridRegular_t        fakeGrid;
	gridPointInt_t       nProcs;
#  ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#  endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fakeGrid = local_getFakeGrid();
	local_getFakeNProcs(nProcs);
	distrib  = gridRegularDistrib_new(fakeGrid, NULL);
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
	if (distrib->nProcs[0] != distrib->numProcs)
		hasPassed = false;
	for (int i = 1; i < NDIM; i++) {
		if (distrib->nProcs[i] != 1)
			hasPassed = false;
	}
	if (distrib->commCart == MPI_COMM_NULL)
		hasPassed = false;
	gridRegular_del(&fakeGrid);
	gridRegularDistrib_del(&distrib);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
} /* gridRegularDistrib_initMPI_test */

extern bool
gridRegularDistrib_getLocalRank_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularDistrib_t distrib;
	gridRegular_t        fakeGrid;
	gridPointInt_t       nProcs;
	int                  localRank;
#  ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#  endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fakeGrid  = local_getFakeGrid();
	local_getFakeNProcs(nProcs);
	distrib   = gridRegularDistrib_new(fakeGrid, NULL);
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
	localRank = gridRegularDistrib_getLocalRank(distrib);
	if ((localRank < 0) || (localRank >= distrib->numProcs))
		hasPassed = false;
	gridRegular_del(&fakeGrid);
	gridRegularDistrib_del(&distrib);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

#endif

extern bool
gridRegularDistrib_getPatchForRank_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularDistrib_t distrib;
	gridRegular_t        fakeGrid;
	gridPatch_t          patch;
	int                  localRank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t               allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	gridPointInt_t nProcs;
	int            size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fakeGrid = local_getFakeGrid();
	distrib  = gridRegularDistrib_new(fakeGrid, NULL);
#ifdef WITH_MPI
	local_getFakeNProcs(nProcs);
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
	localRank = gridRegularDistrib_getLocalRank(distrib);
#endif
	patch     = gridRegularDistrib_getPatchForRank(distrib, localRank);
	for (int i = 1; i < NDIM; i++) {
		if ((patch->idxLo[i] != 0) || (patch->dims[i] != 31))
			hasPassed = false;
	}
#ifdef WITH_MPI
	if (size == 16) {
		if (rank == 1) {
			if ((patch->idxLo[0] != 2) || (patch->dims[0] != 2))
				hasPassed = false;
		}
		if (rank == 15) {
			if ((patch->idxLo[0] != 30) || (patch->dims[0] != 1))
				hasPassed = false;
		}
	}
#endif

	gridRegular_del(&fakeGrid);
	gridPatch_del(&patch);
	gridRegularDistrib_del(&distrib);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegularDistrib_getPatchForRank_test */

extern bool
gridRegularDistrib_calcIdxsForRank1D_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0) {
		uint32_t idxLo;
		uint32_t idxHi;
		printf("Testing %s... ", __func__);
		gridRegularDistrib_calcIdxsForRank1D(11, 2, 0, &idxLo, &idxHi);
		if ((idxLo != 0) || (idxHi != 5))
			hasPassed = false;
		gridRegularDistrib_calcIdxsForRank1D(11, 2, 1, &idxLo, &idxHi);
		if ((idxLo != 6) || (idxHi != 10))
			hasPassed = false;

		gridRegularDistrib_calcIdxsForRank1D(11, 3, 0, &idxLo, &idxHi);
		if ((idxLo != 0) || (idxHi != 3))
			hasPassed = false;
		gridRegularDistrib_calcIdxsForRank1D(11, 3, 1, &idxLo, &idxHi);
		if ((idxLo != 4) || (idxHi != 7))
			hasPassed = false;
		gridRegularDistrib_calcIdxsForRank1D(11, 3, 2, &idxLo, &idxHi);
		if ((idxLo != 8) || (idxHi != 10))
			hasPassed = false;

		gridRegularDistrib_calcIdxsForRank1D(11, 4, 0, &idxLo, &idxHi);
		if ((idxLo != 0) || (idxHi != 2))
			hasPassed = false;
		gridRegularDistrib_calcIdxsForRank1D(11, 4, 1, &idxLo, &idxHi);
		if ((idxLo != 3) || (idxHi != 5))
			hasPassed = false;
		gridRegularDistrib_calcIdxsForRank1D(11, 4, 2, &idxLo, &idxHi);
		if ((idxLo != 6) || (idxHi != 8))
			hasPassed = false;
		gridRegularDistrib_calcIdxsForRank1D(11, 4, 3, &idxLo, &idxHi);
		if ((idxLo != 9) || (idxHi != 10))
			hasPassed = false;
	}

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegularDistrib_calcIdxsForRank1D_test */

extern bool
gridRegularDistrib_transpose_test(void)
{
	bool                 hasPassed = true;
	int                  rank      = 0;
	gridRegularDistrib_t distrib;
#ifdef WITH_SILO
	gridWriter_t         writer;
#endif
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0) {
		printf("Testing %s... ", __func__);
	}

	distrib = local_getFakeDistribForTranspose();
#ifdef WITH_SILO
	writer  = (gridWriter_t)gridWriterSilo_new("transposeTest-stage1",
	                                           DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, distrib->grid);
	gridWriterSilo_deactivate(writer);
	gridWriterSilo_del(&writer);
#endif

	gridRegularDistrib_transpose(distrib, 0, 1);
#ifdef WITH_SILO
	writer = (gridWriter_t)gridWriterSilo_new("transposeTest-stage2",
	                                          DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, distrib->grid);
	gridWriterSilo_deactivate(writer);
	gridWriterSilo_del(&writer);
#endif

	hasPassed = local_verifyFakeDistribForTranspose(distrib);

	gridRegularDistrib_transpose(distrib, 0, 1);
#ifdef WITH_SILO
	writer = (gridWriter_t)gridWriterSilo_new("transposeTest-stage3",
	                                          DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, distrib->grid);
	gridWriterSilo_deactivate(writer);
	gridWriterSilo_del(&writer);
#endif

	gridRegularDistrib_transpose(distrib, 0, 2);
#ifdef WITH_SILO
	writer = (gridWriter_t)gridWriterSilo_new("transposeTest-stage4",
	                                          DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, distrib->grid);
	gridWriterSilo_deactivate(writer);
	gridWriterSilo_del(&writer);
#endif

	gridRegularDistrib_transpose(distrib, 0, 2);
#ifdef WITH_SILO
	writer = (gridWriter_t)gridWriterSilo_new("transposeTest-stage5",
	                                          DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate(writer);
	gridWriterSilo_writeGridRegular(writer, distrib->grid);
	gridWriterSilo_deactivate(writer);
	gridWriterSilo_del(&writer);
#endif

	gridRegularDistrib_del(&distrib);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridRegularDistrib_transpose_test */

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getFakeGrid(void)
{
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = 1.0;
		dims[i]   = 31;
	}

	return gridRegular_new("bla", origin, extent, dims);
}

#ifdef WITH_MPI
static void
local_getFakeNProcs(gridPointInt_t nProcs)
{
	int size;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	for (int i = 0; i < NDIM; i++)
		nProcs[i] = 0;

	nProcs[0] = size;
}

#endif

static gridRegularDistrib_t
local_getFakeDistribForTranspose(void)
{
	gridRegularDistrib_t distrib;
	gridRegular_t        grid;
	gridPointDbl_t       origin;
	gridPointDbl_t       extent;
	gridPointUint32_t    dims;
	gridPointInt_t       nProcs;
	gridPatch_t          patch;
	dataVar_t            var;
	int                  rank = 0;

	origin[0] = 0.0;
	extent[0] = 4.0;
	dims[0]   = 400;
	nProcs[0] = 0;
	origin[1] = 0.0;
	extent[1] = 5.0;
	dims[1]   = 50;
	nProcs[1] = 0;
#if (NDIM > 2)
	origin[2] = 0.0;
	extent[2] = 2.0;
	dims[2]   = 30;
	nProcs[2] = 0;
#endif
	grid      = gridRegular_new("bla", origin, extent, dims);
	var       = dataVar_new("blaVar", DATAVARTYPE_INT, 1);
	gridRegular_attachVar(grid, var);
	distrib   = gridRegularDistrib_new(grid, nProcs);
#ifdef WITH_MPI
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
	rank = gridRegularDistrib_getLocalRank(distrib);
#endif

	patch = gridRegularDistrib_getPatchForRank(distrib, rank);
	gridRegular_attachPatch(grid, patch);
	local_fillFakeGridForTranspose(grid);

	gridRegular_del(&grid);

	return distrib;
} /* local_getFakeDistribForTranspose */

static void
local_fillFakeGridForTranspose(gridRegular_t grid)
{
	gridPatch_t       patch;
	int               *data;
	gridPointUint32_t idxLo;
	gridPointUint32_t dims;
	gridPointUint32_t dimsGlobal;
	uint64_t          offset = 0;

	patch = gridRegular_getPatchHandle(grid, 0);
	gridPatch_getDims(patch, dims);
	gridPatch_getIdxLo(patch, idxLo);
	data = gridPatch_getVarDataHandle(patch, 0);
	gridRegular_getDims(grid, dimsGlobal);

#if (NDIM == 2)
	for (int j = 0; j < dims[1]; j++) {
		for (int i = 0; i < dims[0]; i++) {
			data[offset] = i + idxLo[0]
			               + (j + idxLo[1]) * dimsGlobal[0];
			offset++;
		}
	}
#elif (NDIM == 3)
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				data[offset] = i + idxLo[0]
				               + (j + idxLo[1]) * dimsGlobal[0]
				               + (k + idxLo[2]) * dimsGlobal[0]
				               * dimsGlobal[1];
				offset++;
			}
		}
	}
#endif
} /* local_fillFakeGridForTranspose */

static bool
local_verifyFakeDistribForTranspose(gridRegularDistrib_t distrib)
{
	gridPatch_t       patch;
	int               *data;
	gridPointUint32_t idxLo;
	gridPointUint32_t dims;
	gridPointUint32_t dimsGlobal;
	uint64_t          offset = 0;

	patch = gridRegular_getPatchHandle(distrib->grid, 0);
	gridPatch_getDims(patch, dims);
	gridPatch_getIdxLo(patch, idxLo);
	data = gridPatch_getVarDataHandle(patch, 0);
	gridRegular_getDims(distrib->grid, dimsGlobal);

#if (NDIM == 2)
	for (int j = 0; j < dims[1]; j++) {
		for (int i = 0; i < dims[0]; i++) {
			int expected = j + idxLo[1]
			               + (i + idxLo[0]) * dimsGlobal[1];
			if (data[offset] != expected)
				return false;

			offset++;
		}
	}
#elif (NDIM == 3)
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int expected = j + idxLo[1]
				               + (i + idxLo[0]) * dimsGlobal[1]
				               + (k + idxLo[2]) * dimsGlobal[0]
				               * dimsGlobal[1];
				if (data[offset] != expected)
					return false;

				offset++;
			}
		}
	}
#endif

	return true;
} /* local_verifyFakeDistribForTranspose */
