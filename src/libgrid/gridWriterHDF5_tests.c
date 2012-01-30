// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterHDF5_tests.c
 * @ingroup  libgridIOOutHDF5
 * @brief  Implements the tests for gridWriterHDF5.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterHDF5_tests.h"
#include "gridWriterHDF5.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif
#include "gridRegular.h"
#include "gridRegularDistrib.h"
#include "gridPatch.h"
#include "../libdata/dataVar.h"


/*--- Implementation of main structure ----------------------------------*/
#include "gridWriterHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getFakeGrid(void);

static void
local_fillPatchWithIdxOfCells(gridPatch_t patch, gridPointUint32_t dimsGrid);


/*--- Implementations of exported functions -----------------------------*/
extern bool
gridWriterHDF5_new_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterHDF5_t writer;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	writer = gridWriterHDF5_new();
	if (writer->doChunking)
		hasPassed = false;
	gridWriterHDF5_del((gridWriter_t *)&writer);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridWriterHDF5_del_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterHDF5_t writer;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	writer = gridWriterHDF5_new();
	gridWriterHDF5_del((gridWriter_t *)&writer);
	if (writer != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridWriterHDF5_writeGridRegular_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridWriterHDF5_t  writer;
	gridPointUint32_t chunkSize = { 4, 4, 2 };
	gridRegular_t     grid;
	filename_t        fn;
#ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid   = local_getFakeGrid();

	writer = gridWriterHDF5_new();
	fn = filename_newFull(NULL, "outGridSimple", NULL, ".h5");
	gridWriter_setFileName((gridWriter_t)writer, fn);
	gridWriter_setOverwriteFileIfExists((gridWriter_t)writer, true);
#ifdef WITH_MPI
	gridWriterHDF5_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
#endif
	gridWriterHDF5_activate((gridWriter_t)writer);
	gridWriterHDF5_writeGridRegular((gridWriter_t)writer, grid);
	gridWriterHDF5_deactivate((gridWriter_t)writer);
	gridWriterHDF5_del((gridWriter_t *)&writer);

	writer = gridWriterHDF5_new();
	fn = filename_newFull(NULL, "outGridChunking", NULL, ".h5");
	gridWriter_setFileName((gridWriter_t)writer, fn);
	gridWriter_setOverwriteFileIfExists((gridWriter_t)writer, true);
	gridWriterHDF5_setChunkSize(writer, chunkSize);
#ifdef WITH_MPI
	gridWriterHDF5_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
#endif
	gridWriterHDF5_activate((gridWriter_t)writer);
	gridWriterHDF5_writeGridRegular((gridWriter_t)writer, grid);
	gridWriterHDF5_deactivate((gridWriter_t)writer);
	gridWriterHDF5_del((gridWriter_t *)&writer);

	writer = gridWriterHDF5_new();
	fn = filename_newFull(NULL, "outGridChunkingCompress", NULL, ".h5");
	gridWriter_setFileName((gridWriter_t)writer, fn);
	gridWriter_setOverwriteFileIfExists((gridWriter_t)writer, true);
	gridWriterHDF5_setChunkSize(writer, chunkSize);
	gridWriterHDF5_setDoChecksum(writer, true);
	gridWriterHDF5_setCompressionFilter(writer, "gzip");
#ifdef WITH_MPI
	gridWriterHDF5_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
#endif
	gridWriterHDF5_activate((gridWriter_t)writer);
	gridWriterHDF5_writeGridRegular((gridWriter_t)writer, grid);
	gridWriterHDF5_deactivate((gridWriter_t)writer);
	gridWriterHDF5_del((gridWriter_t *)&writer);

	gridRegular_del(&grid);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridWriterHDF5_writeGridRegular_test */

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getFakeGrid(void)
{
	dataVar_t            var;
	int                  rank;
	gridRegular_t        grid;
	gridRegularDistrib_t gridDistrib;
	gridPatch_t          patch;
	gridPointDbl_t       origin = { 0., 0., 0. };
	gridPointDbl_t       extent = { 1., 1., 1. };
	gridPointUint32_t    dims   = { 4, 8, 16 };


	var         = dataVar_new("FakeVar", DATAVARTYPE_DOUBLE, 1);
	grid        = gridRegular_new("Fake", origin, extent, dims);
	gridRegular_attachVar(grid, var);
	gridDistrib = gridRegularDistrib_new(grid, NULL);
#ifdef WITH_MPI
	gridPointInt_t nProcs = { 1, 2, 1 };
	MPI_Comm_size(MPI_COMM_WORLD, nProcs + 2);
	nProcs[2] /= 2;
	gridRegularDistrib_initMPI(gridDistrib, nProcs, MPI_COMM_WORLD);
#endif
	rank  = gridRegularDistrib_getLocalRank(gridDistrib);
	patch = gridRegularDistrib_getPatchForRank(gridDistrib, rank);
	gridRegular_attachPatch(grid, patch);
	local_fillPatchWithIdxOfCells(patch, dims);

	gridRegularDistrib_del(&gridDistrib);

	return grid;
} /* local_getFakeGrid */

static void
local_fillPatchWithIdxOfCells(gridPatch_t patch, gridPointUint32_t dimsGrid)
{
	gridPointUint32_t idxLo;
	gridPointUint32_t dims;
	double            *data;

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);

	data = gridPatch_getVarDataHandle(patch, 0);
	assert(data != NULL);

	for (uint32_t z = 0; z < dims[2]; z++) {
		for (uint32_t y = 0; y < dims[1]; y++) {
			for (uint32_t x = 0; x < dims[0]; x++) {
				uint64_t idxPatch = x + y * dims[0] + z * dims[0] * dims[1];
				uint64_t idxGrid  = (x + idxLo[0])
				                    + (y + idxLo[1]) * dimsGrid[0]
				                    + (z + idxLo[2]) * dimsGrid[0]
				                    * dimsGrid[1];
				data[idxPatch] = idxGrid;
			}
		}
	}
}
