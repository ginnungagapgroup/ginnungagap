// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_tests.h"
#include "gridReader.h"
#include "gridReaderBov.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridReader_newFromIni_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	parse_ini_t  ini;
	gridReader_t reader;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");

	reader = gridReader_newFromIni(ini, "ReaderPlain");
	gridReader_del(&reader);
	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReader_del_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	parse_ini_t  ini;
	gridReader_t reader;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");

	reader = gridReader_newFromIni(ini, "ReaderPlain");
	gridReader_del(&reader);
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
gridReader_readIntoPatch_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	parse_ini_t  ini;
	gridReader_t reader;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");

	reader = gridReader_newFromIni(ini, "ReaderPlain");

	if (reader->func->readIntoPatch != &gridReaderBov_readIntoPatch)
		hasPassed = false;

	gridReader_del(&reader);
	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReader_readIntoPatchForVar_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	parse_ini_t  ini;
	gridReader_t reader;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini    = parse_ini_open("tests/reading.ini");

	reader = gridReader_newFromIni(ini, "ReaderPlain");

	if (reader->func->readIntoPatchForVar !=
	    &gridReaderBov_readIntoPatchForVar)
		hasPassed = false;

	gridReader_del(&reader);
	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
