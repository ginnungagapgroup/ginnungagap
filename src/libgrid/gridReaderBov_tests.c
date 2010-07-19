// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderBov_tests.h"
#include "gridReaderBov.h"
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


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridReaderBov_newFromIni_test(void)
{
	bool            hasPassed = true;
	int             rank      = 0;
	parse_ini_t     ini;
	gridReaderBov_t reader;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");

	reader = gridReaderBov_newFromIni(ini, "BovDetails");
	gridReaderBov_del((gridReader_t *)&reader);
	parse_ini_close(&ini);
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
	parse_ini_t     ini;
	gridReaderBov_t reader;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");

	reader = gridReaderBov_newFromIni(ini, "BovDetails");
	gridReaderBov_del((gridReader_t *)&reader);
	if (reader != NULL)
		hasPassed = false;
	parse_ini_close(&ini);
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
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif
	uint32_t        idxLo[3]       = { 0, 0, 0 };
	uint32_t        idxHi[3]       = { 7, 7, 7 };
	gridPatch_t     patch          = gridPatch_new(idxLo, idxHi);
	gridReaderBov_t reader;
	parse_ini_t     ini;
	double          *data;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");
	reader = gridReaderBov_newFromIni(ini, "BovDetails");

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
	parse_ini_close(&ini);
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridReaderBov_readIntoPatch_test */

extern bool
gridReaderBov_readIntoPatchForVar_test(void)
{
	bool        hasPassed      = true;
	int         rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
	uint32_t    idxLo[3]       = { 0, 0, 0 };
	uint32_t    idxHi[3]       = { 7, 7, 7 };
	gridPatch_t patch          = gridPatch_new(idxLo, idxHi);
	gridVar_t   var            = gridVar_new("test",
	                                         GRIDVARTYPE_INT,
	                                         3);
	gridReaderBov_t reader;
	parse_ini_t     ini;
	int         *data;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");
	reader = gridReaderBov_newFromIni(ini, "BovDetails");
	gridPatch_attachVar(patch, var);

	gridReaderBov_readIntoPatchForVar((gridReader_t)reader, patch, 0);
	data = gridPatch_getVarDataHandle(patch, 0);
	for (uint64_t i = 0; i < gridPatch_getNumCells(patch); i++) {
		if (data[i*3] != (int)i)
			hasPassed = false;
	}

	gridReaderBov_del((gridReader_t *)&reader);
	parse_ini_close(&ini);
	gridVar_del(&var);
	gridPatch_del(&patch);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* gridReaderBov_readIntoPatchForVar_test */

/*--- Implementations of local functions --------------------------------*/
