// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "grafic_tests.h"
#include "grafic.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "grafic_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
grafic_new_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	if (grafic->isWhiteNoise != false)
		hasPassed = false;
	if (grafic->graficFileName != NULL)
		hasPassed = false;
	grafic_del(&grafic);

	grafic = grafic_new(true);
	if (grafic->isWhiteNoise != true)
		hasPassed = false;
	grafic_del(&grafic);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* grafic_new_test */

extern bool
grafic_newFromFile_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testNormal.grafic");
	if (strcmp(grafic->graficFileName, "tests/testNormal.grafic") != 0)
		hasPassed = false;
	if (grafic->isWhiteNoise != false)
		hasPassed = false;
	grafic_del(&grafic);

	grafic = grafic_newFromFile("tests/testWN.grafic");
	if (strcmp(grafic->graficFileName, "tests/testWN.grafic") != 0)
		hasPassed = false;
	if (grafic->isWhiteNoise != true)
		hasPassed = false;
	grafic_del(&grafic);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* grafic_newFromFile_test */

extern bool
grafic_del_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_del(&grafic);
	if (grafic != NULL)
		hasPassed = false;

	grafic = grafic_newFromFile("tests/testWN.grafic");
	grafic_del(&grafic);
	if (grafic != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getFileName_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testWN.grafic");
	if (strcmp(grafic_getFileName(grafic), "tests/testWN.grafic") != 0)
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getSize_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
	uint32_t size[3];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testWN.grafic");
	grafic_getSize(grafic, size);
	if (size[0] != 8)
		hasPassed = false;
	if (size[1] != 8)
		hasPassed = false;
	if (size[2] != 8)
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getDx_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
	float    dx;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testNormal.grafic");
	dx     = grafic_getDx(grafic);
	if (islessgreater(dx, 1.0f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getXoff_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
	float    xoff[3];
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testNormal.grafic");
	grafic_getXoff(grafic, xoff);
	if (islessgreater(xoff[0], 0.0f))
		hasPassed = false;
	if (islessgreater(xoff[1], 0.0f))
		hasPassed = false;
	if (islessgreater(xoff[2], 0.0f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getAstart_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
	float    astart;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testNormal.grafic");
	astart = grafic_getAstart(grafic);
	if (islessgreater(astart, 0.1f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getOmegam_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
	float    omegam;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testNormal.grafic");
	omegam = grafic_getOmegam(grafic);
	if (islessgreater(omegam, 0.3f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getOmegav_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
	float    omegav;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testNormal.grafic");
	omegav = grafic_getOmegav(grafic);
	if (islessgreater(omegav, 0.7f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getH0_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
	float    h0;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testNormal.grafic");
	h0     = grafic_getH0(grafic);
	if (islessgreater(h0, 72.f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_getIseed_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_newFromFile("tests/testWN.grafic");
	if (grafic_getIseed(grafic) != 666)
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setFileName_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setFileName(grafic, "Hurz");
	if (strcmp(grafic->graficFileName, "Hurz") != 0)
		hasPassed = false;
	grafic_setFileName(grafic, "Hu");
	if (strcmp(grafic->graficFileName, "Hu") != 0)
		hasPassed = false;
	grafic_setFileName(grafic, "Bla");
	if (strcmp(grafic->graficFileName, "Bla") != 0)
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setSize_test(void)
{
	bool     hasPassed      = true;
	int      rank           = 0;
	grafic_t grafic;
	uint32_t size[3]        = { 16, 32, 12 };
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setSize(grafic, size);
	if (grafic->np1 != 16)
		hasPassed = false;
	if (grafic->np2 != 32)
		hasPassed = false;
	if (grafic->np3 != 12)
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setDx_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setDx(grafic, 123.42f);
	if (islessgreater(grafic->dx, 123.42f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setXoff_test(void)
{
	bool     hasPassed      = true;
	int      rank           = 0;
	grafic_t grafic;
	float    xoff[3]        = { 0.0f, 123.f, 1.23f };
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setXoff(grafic, xoff);
	if (islessgreater(grafic->x1o, xoff[0]))
		hasPassed = false;
	if (islessgreater(grafic->x2o, xoff[1]))
		hasPassed = false;
	if (islessgreater(grafic->x3o, xoff[2]))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setAstart_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setAstart(grafic, 0.0423f);
	if (islessgreater(grafic->astart, 0.0423f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setOmegam_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setOmegam(grafic, 0.423f);
	if (islessgreater(grafic->omegam, 0.423f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setOmegav_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setOmegav(grafic, 0.723f);
	if (islessgreater(grafic->omegav, 0.723f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setH0_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(false);
	grafic_setH0(grafic, 23.f);
	if (islessgreater(grafic->h0, 23.f))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_setIseed_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(true);
	grafic_setIseed(grafic, 1244);
	if (grafic->iseed != 1244)
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_isWhiteNoise_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(true);
	if (!grafic_isWhiteNoise(grafic))
		hasPassed = false;
	grafic_del(&grafic);

	grafic = grafic_new(false);
	if (grafic_isWhiteNoise(grafic))
		hasPassed = false;
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_makeEmptyFile_test(void)
{
	bool     hasPassed      = true;
	int      rank           = 0;
	grafic_t grafic;
	uint32_t size[3]        = { 2, 2, 2 };
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(true);
	grafic_setFileName(grafic, "empty.grafic");
	grafic_setSize(grafic, size);
	grafic_makeEmptyFile(grafic);
	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
grafic_read_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
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

	grafic      = grafic_newFromFile("tests/testNormal.grafic");
	grafic_getSize(grafic, size);
	numElements = size[0] * size[1] * size[2];

	data        = xmalloc(sizeof(double) * 2 * numElements);
	grafic_read(grafic, data, GRAFIC_FORMAT_DOUBLE, 2);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(data[i * 2], (double)i))
			hasPassed = false;
	}
	xfree(data);

	data = xmalloc(sizeof(double) * numElements);
	grafic_read(grafic, data, GRAFIC_FORMAT_DOUBLE, 1);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(data[i], (double)i))
			hasPassed = false;
	}
	xfree(data);

	dataFloat = xmalloc(sizeof(float) * 2 * numElements);
	grafic_read(grafic, dataFloat, GRAFIC_FORMAT_FLOAT, 2);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(dataFloat[i * 2], (float)i))
			hasPassed = false;
	}
	xfree(dataFloat);

	dataFloat = xmalloc(sizeof(float) * numElements);
	grafic_read(grafic, dataFloat, GRAFIC_FORMAT_FLOAT, 1);
	for (int i = 0; i < numElements; i++) {
		if (islessgreater(dataFloat[i], (float)i))
			hasPassed = false;
	}
	xfree(dataFloat);

	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* grafic_read_test */

extern bool
grafic_readWindowed_test(void)
{
	bool     hasPassed = true;
	int      rank      = 0;
	grafic_t grafic;
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

	grafic      = grafic_newFromFile("tests/testWN.grafic");
	grafic_getSize(grafic, size);
	numElements = dims[0] * dims[1] * dims[2];

	data        = xmalloc(sizeof(double) * 2 * numElements);
	grafic_readWindowed(grafic, data, GRAFIC_FORMAT_DOUBLE, 2, idxLo, dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int    pos       = i + (j + k * dims[1]) * dims[0];
				double graficPos = (double)((i + idxLo[0])
				                            + ((j + idxLo[1])
				                               + (k + idxLo[2]) * size[1])
				                            * size[0]);
				if (islessgreater(data[pos * 2], graficPos))
					hasPassed = false;
			}
		}
	}
	xfree(data);

	data = xmalloc(sizeof(double) * numElements);
	grafic_readWindowed(grafic, data, GRAFIC_FORMAT_DOUBLE, 1, idxLo, dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int    pos       = i + (j + k * dims[1]) * dims[0];
				double graficPos = (double)((i + idxLo[0])
				                            + ((j + idxLo[1])
				                               + (k + idxLo[2]) * size[1])
				                            * size[0]);
				if (islessgreater(data[pos], graficPos))
					hasPassed = false;
			}
		}
	}
	xfree(data);

	dataFloat = xmalloc(sizeof(float) * 2 * numElements);
	grafic_readWindowed(grafic,
	                    dataFloat,
	                    GRAFIC_FORMAT_FLOAT,
	                    2,
	                    idxLo,
	                    dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int   pos       = i + (j + k * dims[1]) * dims[0];
				float graficPos = (float)((i + idxLo[0])
				                          + ((j + idxLo[1])
				                             + (k + idxLo[2]) * size[1])
				                          * size[0]);
				if (islessgreater(dataFloat[pos * 2], graficPos))
					hasPassed = false;
			}
		}
	}
	xfree(dataFloat);

	dataFloat = xmalloc(sizeof(float) * numElements);
	grafic_readWindowed(grafic,
	                    dataFloat,
	                    GRAFIC_FORMAT_FLOAT,
	                    1,
	                    idxLo,
	                    dims);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int   pos       = i + (j + k * dims[1]) * dims[0];
				float graficPos = (float)((i + idxLo[0])
				                          + ((j + idxLo[1])
				                             + (k + idxLo[2]) * size[1])
				                          * size[0]);
				if (islessgreater(dataFloat[pos], graficPos))
					hasPassed = false;
			}
		}
	}
	xfree(dataFloat);

	grafic_del(&grafic);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* grafic_readWindowed_test */

extern bool
grafic_write_test(void)
{
	bool     hasPassed   = true;
	int      rank        = 0;
	grafic_t grafic;
	uint32_t size[3]     = { 3, 4, 2 };
	size_t   numElements = 3 * 4 * 2;
	float    *data, *data2;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	data = xmalloc(sizeof(float) * numElements);
	for (int i = 0; i < numElements; i++)
		data[i] = (float)i;

	grafic = grafic_new(true);
	grafic_setSize(grafic, size);
	grafic_setFileName(grafic, "writeTest.grafic");
	grafic_write(grafic, data, GRAFIC_FORMAT_FLOAT, 1);
	grafic_del(&grafic);

	data2  = xmalloc(sizeof(float) * numElements);
	grafic = grafic_newFromFile("writeTest.grafic");
	grafic_read(grafic, data2, GRAFIC_FORMAT_FLOAT, 1);
	grafic_del(&grafic);

	for (int i = 0; i < numElements; i++) {
		if (islessgreater(data2[i], data[i]))
			hasPassed = false;
	}

	xfree(data2);
	xfree(data);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* grafic_write_test */

extern bool
grafic_writeWindowed_test(void)
{
	bool     hasPassed   = true;
	int      rank        = 0;
	grafic_t grafic;
	uint32_t idxLo[3]    = { 1, 1, 0 };
	uint32_t dims[3]     = { 2, 2, 1 };
	uint32_t size[3]     = { 4, 5, 2 };
	size_t   numElements = 2 * 2 * 1;
	float    *data;
#ifdef XMEM_TRACK_MEM
	size_t   allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	grafic = grafic_new(true);
	grafic_setSize(grafic, size);
	grafic_setFileName(grafic, "writeWindowed.grafic");
	numElements = dims[0] * dims[1] * dims[2];

	data        = xmalloc(sizeof(double) * 2 * numElements);
	for (int k = 0; k < dims[2]; k++) {
		for (int j = 0; j < dims[1]; j++) {
			for (int i = 0; i < dims[0]; i++) {
				int idx = i + (j + k * dims[1]) * dims[0];
				data[idx] = (float)idx;
			}
		}
	}
	grafic_makeEmptyFile(grafic);
	grafic_writeWindowed(grafic, data, GRAFIC_FORMAT_FLOAT, 1, idxLo, dims);
	grafic_del(&grafic);
	xfree(data);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* grafic_readWindowed_test */

/*--- Implementations of local functions --------------------------------*/
