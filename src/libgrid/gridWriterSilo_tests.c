// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#ifdef WITH_SILO
#  include "gridWriterSilo_tests.h"
#  include "gridWriterSilo.h"
#  include <stdio.h>
#  include <string.h>
#  include <silo.h>
#  ifdef WITH_MPI
#    include <mpi.h>
#  endif
#  ifdef XMEM_TRACK_MEM
#    include "../libutil/xmem.h"
#  endif


/*--- Implemention of main structure ------------------------------------*/
#  include "gridWriterSilo_adt.h"


/*--- Local defines -----------------------------------------------------*/
#  define LOCAL_TESTPREFIX       "siloTest"
#  define LOCAL_TESTPREFIX_PATCH "siloTestPatch"


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_getFakeGridRegular(void);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridWriterSilo_new_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterSilo_t writer;
#  ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#  endif
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	writer = gridWriterSilo_new(LOCAL_TESTPREFIX, DB_HDF5);
	if (strcmp(writer->prefix, LOCAL_TESTPREFIX) != 0)
		hasPassed = false;
	if (writer->dbType != DB_HDF5)
		hasPassed = false;
	gridWriterSilo_del((gridWriter_t *)&writer);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

extern bool
gridWriterSilo_del_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriter_t writer;
#  ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#  endif
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	writer = (gridWriter_t)gridWriterSilo_new(LOCAL_TESTPREFIX, DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel(writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_del(&writer);
	if (writer != NULL)
		hasPassed = false;
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

#  ifdef WITH_MPI
extern bool
gridWriterSilo_initParallel_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	int              size;
	gridWriterSilo_t writer;
#    ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#    endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	writer = gridWriterSilo_new(LOCAL_TESTPREFIX, DB_HDF5);
	gridWriterSilo_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
	if (writer->baton == NULL)
		hasPassed = false;
	if (writer->groupRank != 0)
		hasPassed = false;
	if (writer->rankInGroup != rank)
		hasPassed = false;
	gridWriterSilo_del((gridWriter_t *)&writer);

	writer = gridWriterSilo_new(LOCAL_TESTPREFIX, DB_HDF5);
	writer->numFiles = size;
	gridWriterSilo_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
	if (writer->baton == NULL)
		hasPassed = false;
	if (writer->groupRank != rank)
		hasPassed = false;
	if (writer->rankInGroup != 0)
		hasPassed = false;
	gridWriterSilo_del((gridWriter_t *)&writer);

#    ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#    endif

	return hasPassed ? true : false;
} /* gridWriterSilo_initParallel_test */

#  endif

extern bool
gridWriterSilo_activate_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterSilo_t writer;
#  ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#  endif
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	writer = gridWriterSilo_new(LOCAL_TESTPREFIX, DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate((gridWriter_t)writer);
	if (writer->isActive != true)
		hasPassed = false;
	if (writer->f == NULL)
		hasPassed = false;
	gridWriterSilo_deactivate((gridWriter_t)writer);
	gridWriterSilo_del((gridWriter_t *)&writer);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

extern bool
gridWriterSilo_deactivate_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterSilo_t writer;
#  ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#  endif
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	writer = gridWriterSilo_new(LOCAL_TESTPREFIX, DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate((gridWriter_t)writer);
	gridWriterSilo_deactivate((gridWriter_t)writer);
	if (writer->isActive != false)
		hasPassed = false;
	if (writer->f != NULL)
		hasPassed = false;
	gridWriterSilo_del((gridWriter_t *)&writer);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

extern bool
gridWriterSilo_writeGridPatch_test(void)
{
	bool              hasPassed = true;
	int               rank      = 0;
	gridWriterSilo_t  writer;
	gridPatch_t       patch;
	gridPatch_t       patch2;
	gridPointUint32_t lo;
	gridPointUint32_t hi;
	gridPointUint32_t lo2;
	gridPointUint32_t hi2;
	gridPointDbl_t    origin;
	gridPointDbl_t    delta;
#  ifdef XMEM_TRACK_MEM
	size_t            allocatedBytes = global_allocated_bytes;
#  endif
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	for (int i = 0; i < NDIM; i++) {
		lo[i]     = 0 + (rank) * 32;
		hi[i]     = 31 + (rank) * 32;
		lo2[i]    = 32 + (rank) * 32;
		hi2[i]    = 63 + (rank) * 32;
		origin[i] = 0.0;
		delta[i]  = 1.0;
	}
	patch  = gridPatch_new(lo, hi);
	patch2 = gridPatch_new(lo2, hi2);

	writer = gridWriterSilo_new(LOCAL_TESTPREFIX_PATCH, DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate((gridWriter_t)writer);
	gridWriterSilo_writeGridPatch((gridWriter_t)writer, patch,
	                              "patch_00", origin, delta);
	gridWriterSilo_writeGridPatch((gridWriter_t)writer, patch2,
	                              "patch_01", origin, delta);
	gridWriterSilo_deactivate((gridWriter_t)writer);
	gridWriterSilo_del((gridWriter_t *)&writer);

	gridPatch_del(&patch2);
	gridPatch_del(&patch);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
} /* gridWriterSilo_writeGridPatch_test */

extern bool
gridWriterSilo_writeGridRegular_test(void)
{
	bool             hasPassed = true;
	int              rank      = 0;
	gridWriterSilo_t writer;
	gridRegular_t    grid;
#  ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#  endif
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grid   = local_getFakeGridRegular();
	// TODO Initialize memory areas

	writer = gridWriterSilo_new(LOCAL_TESTPREFIX, DB_HDF5);
#  ifdef WITH_MPI
	gridWriterSilo_initParallel((gridWriter_t)writer, MPI_COMM_WORLD);
#  endif
	gridWriterSilo_activate((gridWriter_t)writer);
	gridWriterSilo_writeGridRegular((gridWriter_t)writer, grid);
	gridWriterSilo_deactivate((gridWriter_t)writer);
	gridWriterSilo_del((gridWriter_t *)&writer);
	if (writer != NULL)
		hasPassed = false;

	gridRegular_del(&grid);

#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_getFakeGridRegular(void)
{
	gridRegular_t     grid;
	gridPointDbl_t    origin;
	gridPointDbl_t    extent;
	gridPointUint32_t dims;
	gridPointUint32_t lo;
	gridPointUint32_t hi;
	gridPatch_t patch;
	dataVar_t var;
	int rank = 0;
	int size = 1;
	int perRank;
#ifdef WITH_MPI
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = 1.0;
		dims[i]   = 128;
		lo[i] = 0;
		hi[i] = 127;
	}
	perRank = 128/size;

	grid = gridRegular_new("TestGrid", origin, extent, dims);

	lo[0] = rank * perRank;
	hi[0] = lo[0] + perRank/2 - 1;
	patch = gridPatch_new(lo, hi);
	gridRegular_attachPatch(grid, patch);
	lo[0] += perRank/2;
	hi[0] = lo[0] + perRank/2 - 1;
	patch = gridPatch_new(lo, hi);
	gridRegular_attachPatch(grid, patch);

	var = dataVar_new("var1", DATAVARTYPE_DOUBLE, 1);
	gridRegular_attachVar(grid, var);
	var = dataVar_new("var2", DATAVARTYPE_DOUBLE, 3);
	gridRegular_attachVar(grid, var);

	return grid;
}

#endif
