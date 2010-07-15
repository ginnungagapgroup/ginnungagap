// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "bov_tests.h"
#include "bov.h"
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
#include "bov_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
bov_new_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_newFromFile_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_del_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_del(&bov);
	if (bov != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getTime_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
	double time;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov  = bov_newFromFile("tests/test_1.bov");
	time = bov_getTime(bov);
	if (islessgreater(time, 2.2))
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getDataFileName_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
	char   *dataFileName;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov          = bov_newFromFile("tests/test_1.bov");
	dataFileName = bov_getDataFileName(bov);
	if (strcmp(dataFileName, "tests/data.dat") != 0)
		hasPassed = false;
	xfree(dataFileName);
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getDataSize_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	bov_t    bov;
	uint32_t dataSize[3];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	bov_getDataSize(bov, dataSize);
	if (dataSize[0] != 8)
		hasPassed = false;
	if (dataSize[1] != 8)
		hasPassed = false;
	if (dataSize[2] != 8)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getDataFormat_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	if (bov_getDataFormat(bov) != BOV_FORMAT_DOUBLE)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getVarName_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
	char   *varName;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov     = bov_newFromFile("tests/test_1.bov");
	varName = bov_getVarName(bov);
	if (strcmp(varName, "testVar") != 0)
		hasPassed = false;
	xfree(varName);
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getDataEndian_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	if (bov_getDataEndian(bov) != BOV_ENDIAN_BIG)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getCentering_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	if (bov_getCentering(bov) != BOV_CENTERING_ZONAL)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getBrickOrigin_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
	double origin[3];
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	bov_getBrickOrigin(bov, origin);
	if (islessgreater(origin[0], -4.))
		hasPassed = false;
	if (islessgreater(origin[1], -4.))
		hasPassed = false;
	if (islessgreater(origin[2], -4.))
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getBrickSize_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
	double size[3];
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	bov_getBrickSize(bov, size);
	if (islessgreater(size[0], 8.))
		hasPassed = false;
	if (islessgreater(size[1], 8.))
		hasPassed = false;
	if (islessgreater(size[2], 8.))
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_getDataComponents_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_newFromFile("tests/test_1.bov");
	if (bov_getDataComponents(bov) != 2)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setTime_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setTime(bov, 1.0);
	if (islessgreater(bov->time, 1.0))
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setDataFileName_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setTime(bov, 1.0);
	if (islessgreater(bov->time, 1.0))
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setDataSize_test(void)
{
	bool     hasPassed      = true;
	int      rank           = 0;
	bov_t    bov;
	uint32_t dataSize[3]    = { 45, 45, 45 };
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setDataSize(bov, dataSize);
	if (bov->data_size[0] != dataSize[0])
		hasPassed = false;
	if (bov->data_size[1] != dataSize[1])
		hasPassed = false;
	if (bov->data_size[2] != dataSize[2])
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setDataFormat_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setDataFormat(bov, BOV_FORMAT_BYTE);
	if (bov->data_format != BOV_FORMAT_BYTE)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setVarName_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setVarName(bov, "Hoppla");
	if (strcmp(bov->variable, "Hoppla") != 0)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setDataEndian_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setDataEndian(bov, BOV_ENDIAN_BIG);
	if (bov->data_endian != BOV_ENDIAN_BIG)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setCentering_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setCentering(bov, BOV_CENTERING_NODAL);
	if (bov->centering != BOV_CENTERING_NODAL)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setBrickOrigin_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	bov_t  bov;
	double origin[3]      = { -1.1, -3.1, -2.1 };
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setBrickOrigin(bov, origin);
	if (islessgreater(bov->brick_origin[0], origin[0]))
		hasPassed = false;
	if (islessgreater(bov->brick_origin[1], origin[1]))
		hasPassed = false;
	if (islessgreater(bov->brick_origin[2], origin[2]))
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setBrickSize_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	bov_t  bov;
	double size[3]        = { 1.1, 3.1, 2.1 };
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setBrickSize(bov, size);
	if (islessgreater(bov->brick_size[0], size[0]))
		hasPassed = false;
	if (islessgreater(bov->brick_size[1], size[1]))
		hasPassed = false;
	if (islessgreater(bov->brick_size[2], size[2]))
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_setDataComponents_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	bov_t  bov;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov = bov_new();
	bov_setDataComponents(bov, 4);
	if (bov->data_components != 4)
		hasPassed = false;
	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
bov_read_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	bov_t    bov;
	uint32_t size[3];
	size_t   numElements;
	double   *data;
	float    *dataFloat;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov         = bov_newFromFile("tests/test_1.bov");
	bov_getDataSize(bov, size);
	numElements = size[0] * size[1] * size[2];

	data        = xmalloc(sizeof(double) * 2 * numElements);
	bov_read(bov, data, BOV_FORMAT_DOUBLE, 2);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(data[i * 2], (double)i))
			hasPassed = false;
		if (isless(data[i * 2 + 1], 0.0) || isgreater(data[i * 2 + 1], 1.0))
			hasPassed = false;
	}
	xfree(data);

	data = xmalloc(sizeof(double) * numElements);
	bov_read(bov, data, BOV_FORMAT_DOUBLE, 1);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(data[i], (double)i))
			hasPassed = false;
	}
	xfree(data);

	dataFloat = xmalloc(sizeof(float) * 2 * numElements);
	bov_read(bov, dataFloat, BOV_FORMAT_FLOAT, 2);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(dataFloat[i * 2], (float)i))
			hasPassed = false;
		if (isless(dataFloat[i * 2 + 1], 0.0)
		    || isgreater(dataFloat[i * 2 + 1], 1.0))
			hasPassed = false;
	}
	xfree(dataFloat);

	dataFloat = xmalloc(sizeof(float) * numElements);
	bov_read(bov, dataFloat, BOV_FORMAT_FLOAT, 1);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(dataFloat[i], (float)i))
			hasPassed = false;
	}
	xfree(dataFloat);

	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* bov_read_test */

extern bool
bov_readWindowed_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	bov_t    bov;
	uint32_t idxLo[3]  = { 1, 1, 1 };
	uint32_t dims[3]   = { 3, 2, 1 };
	uint32_t size[3];
	size_t   numElements;
	double   *data;
	float    *dataFloat;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	bov         = bov_newFromFile("tests/test_1.bov");
	bov_getDataSize(bov, size);
	numElements = dims[0] * dims[1] * dims[2];

	data        = xmalloc(sizeof(double) * 2 * numElements);
	bov_readWindowed(bov, data, BOV_FORMAT_DOUBLE, 2, idxLo, dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int    pos    = i + (j + k * dims[1]) * dims[0];
				double bovPos = (double)((i + idxLo[0])
				                         + ((j + idxLo[1])
				                            + (k + idxLo[2]) * size[1])
				                         * size[0]);
				if (islessgreater(data[pos * 2], bovPos))
					hasPassed = false;
				if (isless(data[pos * 2 + 1], 0.0)
				    || isgreater(data[pos * 2 + 1], 1.0))
					hasPassed = false;
			}
		}
	}
	xfree(data);

	data = xmalloc(sizeof(double) * numElements);
	bov_readWindowed(bov, data, BOV_FORMAT_DOUBLE, 1, idxLo, dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int    pos    = i + (j + k * dims[1]) * dims[0];
				double bovPos = (double)((i + idxLo[0])
				                         + ((j + idxLo[1])
				                            + (k + idxLo[2]) * size[1])
				                         * size[0]);
				if (islessgreater(data[pos], bovPos))
					hasPassed = false;
			}
		}
	}
	xfree(data);

	dataFloat = xmalloc(sizeof(float) * 2 * numElements);
	bov_readWindowed(bov, dataFloat, BOV_FORMAT_FLOAT, 2, idxLo, dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int   pos    = i + (j + k * dims[1]) * dims[0];
				float bovPos = (float)((i + idxLo[0])
				                       + ((j + idxLo[1])
				                          + (k + idxLo[2]) * size[1])
				                       * size[0]);
				if (islessgreater(dataFloat[pos * 2], bovPos))
					hasPassed = false;
				if (isless(dataFloat[pos * 2 + 1], 0.0)
				    || isgreater(dataFloat[pos * 2 + 1], 1.0))
					hasPassed = false;
			}
		}
	}
	xfree(dataFloat);

	dataFloat = xmalloc(sizeof(float) * numElements);
	bov_readWindowed(bov, dataFloat, BOV_FORMAT_FLOAT, 1, idxLo, dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int   pos    = i + (j + k * dims[1]) * dims[0];
				float bovPos = (float)((i + idxLo[0])
				                       + ((j + idxLo[1])
				                          + (k + idxLo[2]) * size[1])
				                       * size[0]);
				if (islessgreater(dataFloat[pos], bovPos))
					hasPassed = false;
			}
		}
	}
	xfree(dataFloat);

	bov_del(&bov);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* bov_readWindowed_test */

/*--- Implementations of local functions --------------------------------*/
