// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderBov_tests.c
 * @ingroup  libgridIOInBovTests
 * @brief  Implements the tests for the BOV reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderBov_tests.h"
#include "gridReaderBov.h"
#include "gridReaderFactory.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridReaderBov_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridReaderBov_t
local_getReader(void);


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridReaderBov_new_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	gridReaderBov_t reader;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = gridReaderBov_new();
	if ((reader->base).type != GRIDIO_TYPE_BOV)
		hasPassed = false;
	if ((reader->base).fileName != NULL)
		hasPassed = false;
	if (reader->bov != NULL)
		hasPassed = false;

	gridReaderBov_del(&reader);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderBov_del_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	gridReaderBov_t reader;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = local_getReader();
	gridReaderBov_del((gridReader_t *)&reader);
	if (reader != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderBov_getBov_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	gridReaderBov_t reader;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = local_getReader();

	if (gridReaderBov_getBov(reader) != reader->bov)
		hasPassed = false;

	gridReaderBov_del((gridReader_t *)&reader);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderBov_readIntoPatch_test(void)
{
	bool            hasPassed      = true;
	int             rank           = 0;
#  ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#  endif
	uint32_t        idxLo[3]       = {0, 0, 0};
	uint32_t        idxHi[3]       = {7, 7, 7};
	gridPatch_t     patch          = gridPatch_new(idxLo, idxHi);
	gridReaderBov_t reader;
	double          *data;
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = local_getReader();

	gridReaderBov_readIntoPatch((gridReader_t)reader, patch);
	data = gridPatch_getVarDataHandle(patch, 0);
	for (uint64_t i = 0; i < gridPatch_getNumCells(patch); i++) {
		if (islessgreater(data[i * 2], (double)i))
			hasPassed = false;
		if (isless(data[i * 2 + 1], 0.0)
		    || isgreater(data[i * 2 + 1], 1.0))
			hasPassed = false;
	}

	gridReaderBov_del((gridReader_t *)&reader);
	gridPatch_del(&patch);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

extern bool
gridReaderBov_readIntoPatchForVar_test(void)
{
	bool        hasPassed      = true;
	int         rank           = 0;
#  ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#  endif
	uint32_t    idxLo[3]       = {0, 0, 0};
	uint32_t    idxHi[3]       = {7, 7, 7};
	gridPatch_t patch          = gridPatch_new(idxLo, idxHi);
	dataVar_t   var            = dataVar_new("test", DATAVARTYPE_INT, 3);
	gridReaderBov_t reader;
	int             *data;
#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#  endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = local_getReader();
	gridPatch_attachVar(patch, var);

	gridReaderBov_readIntoPatchForVar((gridReader_t)reader, patch, 0);
	data = gridPatch_getVarDataHandle(patch, 0);
	for (uint64_t i = 0; i < gridPatch_getNumCells(patch); i++) {
		if (data[i * 3] != (int)i)
			hasPassed = false;
	}

	gridReaderBov_del((gridReader_t *)&reader);
	dataVar_del(&var);
	gridPatch_del(&patch);
#  ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#  endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
static gridReaderBov_t
local_getReader(void)
{
	gridReaderBov_t reader;
	parse_ini_t     ini;

	ini    = parse_ini_open("tests/reading.ini");
	reader = (gridReaderBov_t)
	         gridReaderFactory_newReaderFromIni(ini, "ReaderBov");
	parse_ini_close(&ini);

	return reader;
}
