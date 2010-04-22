// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "commScheme_tests.h"
#include "commScheme.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "commScheme_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static commScheme_t
local_getFakeScheme(int **buffers);


/*--- Implementations of exported functios ------------------------------*/
extern bool
commScheme_new_test(void)
{
	bool         hasPassed = true;
	int          rank;
	commScheme_t scheme;
	int          *buffers;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	scheme = local_getFakeScheme(&buffers);
	if (scheme->rank != rank)
		hasPassed = false;
	if (scheme->requestsRecv != NULL)
		hasPassed = false;
	if (scheme->requestsSend != NULL)
		hasPassed = false;
	commScheme_del(&scheme);
	xfree(buffers);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} 

extern bool
commScheme_del_test(void)
{
	bool         hasPassed = true;
	int          rank;
	commScheme_t scheme;
	int          *buffers;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	scheme = local_getFakeScheme(&buffers);
	if (scheme->rank != rank)
		hasPassed = false;
	if (scheme->requestsRecv != NULL)
		hasPassed = false;
	if (scheme->requestsSend != NULL)
		hasPassed = false;
	commScheme_del(&scheme);
	xfree(buffers);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commScheme_addBuffer_test(void)
{
	bool         hasPassed = true;
	int          rank;
	commScheme_t scheme;
	int          *buffers;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	scheme = local_getFakeScheme(&buffers);
	if (varArr_getLength(scheme->buffersRecv) != 1)
		hasPassed = false;
	if (varArr_getLength(scheme->buffersSend) != 1)
		hasPassed = false;
	commScheme_del(&scheme);
	xfree(buffers);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commScheme_fire_test(void)
{
	bool         hasPassed = true;
	int          rank;
	commScheme_t scheme;
	int          *buffers;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	scheme = local_getFakeScheme(&buffers);
	commScheme_fire(scheme);
	commScheme_wait(scheme);
	if (buffers[0] != (rank + scheme->size - 1) % scheme->size)
		hasPassed = false;
	commScheme_del(&scheme);
	xfree(buffers);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commScheme_fireBlock_test(void)
{
	bool         hasPassed = true;
	int          rank;
	commScheme_t scheme;
	int          *buffers;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	scheme = local_getFakeScheme(&buffers);
	commScheme_fireBlock(scheme);
	if (buffers[0] != (rank + scheme->size - 1) % scheme->size)
		hasPassed = false;
	commScheme_del(&scheme);
	xfree(buffers);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commScheme_wait_test(void)
{
	bool         hasPassed = true;
	int          rank;
	commScheme_t scheme;
	int          *buffers;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		printf("Testing %s... ", __func__);

	scheme = local_getFakeScheme(&buffers);
	commScheme_fireBlock(scheme);
	if (scheme->status != COMMSCHEME_STATUS_POSTFIRE)
		hasPassed = false;
	if (scheme->requestsRecv != NULL)
		hasPassed = false;
	if (scheme->requestsSend != NULL)
		hasPassed = false;
	commScheme_del(&scheme);
	xfree(buffers);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
static commScheme_t
local_getFakeScheme(int **buffers)
{
	commScheme_t       scheme;
	commSchemeBuffer_t buf;
	int                to, from;

	scheme        = commScheme_new(MPI_COMM_WORLD, 99);
	*buffers      = xmalloc(sizeof(int) * 1);
	to            = scheme->rank + 1;
	to            = (to >= scheme->size) ? 0 : to;
	buf           = commSchemeBuffer_new(&(scheme->rank), 1, MPI_INT, to);

	commScheme_addBuffer(scheme, buf, COMMSCHEME_TYPE_SEND);
	(*buffers)[0] = -1;
	from     = scheme->rank - 1;
	from     = (from < 0) ? scheme->size - 1 : from;
	buf      = commSchemeBuffer_new(*buffers, 1, MPI_INT, from);
	commScheme_addBuffer(scheme, buf, COMMSCHEME_TYPE_RECV);

	return scheme;
}
