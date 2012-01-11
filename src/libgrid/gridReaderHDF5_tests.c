// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5_tests.c
 * @ingroup  libgridIOInHDF5
 * @brief  Implements the test for gridReaderHDF5.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderHDF5_tests.h"
#include "gridReaderHDF5.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "gridReaderHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
gridReaderHDF5_newFromIni_test(void)
{
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
	bool             hasPassed = true;
	int              rank      = 0;
	parse_ini_t      ini       = parse_ini_open("tests/hdf5.ini");
	gridReaderHDF5_t reader;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = gridReaderHDF5_newFromIni(ini, "Test1");
	if (strcmp(reader->fileName, "tests/hdf5_simple.h5") != 0)
		hasPassed = false;

	gridReaderHDF5_del((gridReader_t *)&reader);
	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderHDF5_del_test(void)
{
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
	bool             hasPassed = true;
	int              rank      = 0;
	parse_ini_t      ini       = parse_ini_open("tests/hdf5.ini");
	gridReaderHDF5_t reader;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = gridReaderHDF5_newFromIni(ini, "Test1");

	gridReaderHDF5_del((gridReader_t *)&reader);
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
gridReaderHDF5_readIntoPatch_test(void)
{
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
	bool             hasPassed = true;
	int              rank      = 0;
	parse_ini_t      ini       = parse_ini_open("tests/hdf5.ini");
	gridReaderHDF5_t reader;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = gridReaderHDF5_newFromIni(ini, "Test1");

	gridReaderHDF5_del((gridReader_t *)&reader);
	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
gridReaderHDF5_readIntoPatchForVar_test(void)
{
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
	bool             hasPassed = true;
	int              rank      = 0;
	parse_ini_t      ini       = parse_ini_open("tests/hdf5.ini");
	gridReaderHDF5_t reader;
	gridPatch_t      patch;
	dataVar_t        var;
	double           *data;
	gridPointUint32_t idxLo = {1, 1, 1};
	gridPointUint32_t idxHi = {3, 7, 15};
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	reader = gridReaderHDF5_newFromIni(ini, "Test1");
	var = dataVar_new("FakeVar", DATAVARTYPE_DOUBLE, 1);
	patch = gridPatch_new(idxLo, idxHi);
	gridPatch_attachVar(patch, var);


	gridReaderHDF5_readIntoPatchForVar((gridReader_t)reader, patch, 0);
	data = (double *)gridPatch_getVarDataHandle(patch, 0);
#define IDXDATA(x,y,z) ((x) + ((y) + (z) * (idxHi[1] + 1 - idxLo[1])) * (idxHi[0] + 1 - idxLo[1]))
#define IDXREAL(x,y,z) ((x + idxLo[0]) + ((y + idxLo[1]) + (z + idxLo[2]) * 8) * 4)
	if (islessgreater(data[IDXDATA(0,0,0)], IDXREAL(0,0,0)))
		hasPassed = false;
	if (islessgreater(data[IDXDATA(2,1,5)], IDXREAL(2,1,5)))
		hasPassed = false;

	gridReaderHDF5_del((gridReader_t *)&reader);
	dataVar_del(&var);
	gridPatch_del(&patch);
	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
