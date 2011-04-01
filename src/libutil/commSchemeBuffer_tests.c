// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "commSchemeBuffer_tests.h"
#include "commSchemeBuffer.h"
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "commSchemeBuffer_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
commSchemeBuffer_new_test(void)
{
	bool               hasPassed = true;
	int                rank      = 0;
	commSchemeBuffer_t buf;
#ifdef XMEM_TRACK_MEM
	size_t             allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	buf = commSchemeBuffer_new(&rank, 1, MPI_INT, rank);
	if (buf->buf != &rank)
		hasPassed = false;
	if (buf->count != 1)
		hasPassed = false;
	if (buf->datatype != MPI_INT)
		hasPassed = false;
	if (buf->rank != rank)
		hasPassed = false;
	commSchemeBuffer_del(&buf);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commSchemeBuffer_del_test(void)
{
	bool               hasPassed = true;
	int                rank      = 0;
	commSchemeBuffer_t buf;
#ifdef XMEM_TRACK_MEM
	size_t             allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	buf = commSchemeBuffer_new(&rank, 1, MPI_INT, rank);
	commSchemeBuffer_del(&buf);
	if (buf != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commSchemeBuffer_getBuf_test(void)
{
	bool               hasPassed = true;
	int                rank      = 0;
	commSchemeBuffer_t buf;
#ifdef XMEM_TRACK_MEM
	size_t             allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	buf = commSchemeBuffer_new(&rank, 1, MPI_INT, rank);
	if (commSchemeBuffer_getBuf(buf) != &rank)
		hasPassed = false;
	commSchemeBuffer_del(&buf);
	if (buf != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commSchemeBuffer_getCount_test(void)
{
	bool               hasPassed = true;
	int                rank      = 0;
	commSchemeBuffer_t buf;
#ifdef XMEM_TRACK_MEM
	size_t             allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	buf = commSchemeBuffer_new(&rank, 1, MPI_INT, rank);
	if (commSchemeBuffer_getCount(buf) != 1)
		hasPassed = false;
	commSchemeBuffer_del(&buf);
	if (buf != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commSchemeBuffer_getDatatype_test(void)
{
	bool               hasPassed = true;
	int                rank      = 0;
	commSchemeBuffer_t buf;
#ifdef XMEM_TRACK_MEM
	size_t             allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	buf = commSchemeBuffer_new(&rank, 1, MPI_INT, rank);
	if (commSchemeBuffer_getDatatype(buf) != MPI_INT)
		hasPassed = false;
	commSchemeBuffer_del(&buf);
	if (buf != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commSchemeBuffer_getRank_test(void)
{
	bool               hasPassed = true;
	int                rank      = 0;
	commSchemeBuffer_t buf;
#ifdef XMEM_TRACK_MEM
	size_t             allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	buf = commSchemeBuffer_new(&rank, 1, MPI_INT, rank);
	if (commSchemeBuffer_getRank(buf) != rank)
		hasPassed = false;
	commSchemeBuffer_del(&buf);
	if (buf != NULL)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
