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
local_getFakeSendScheme(int **buffers);

static commScheme_t
local_getFakeRecvScheme(int **buffers);


/*--- Implementations of exported functios ------------------------------*/
extern bool
commScheme_new_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	commScheme_t schemeSend;
	commScheme_t schemeRecv;
	int          *sendBuffer;
	int          *recvBuffer;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	schemeSend = local_getFakeSendScheme(&sendBuffer);
	schemeRecv = local_getFakeRecvScheme(&recvBuffer);
	if (schemeSend->type != COMMSCHEME_TYPE_SEND)
		hasPassed = false;
	if (schemeSend->requests != NULL)
		hasPassed = false;
	if (schemeRecv->type != COMMSCHEME_TYPE_RECV)
		hasPassed = false;
	if (schemeRecv->requests != NULL)
		hasPassed = false;
	commScheme_del(&schemeRecv);
	commScheme_del(&schemeSend);
	xfree(recvBuffer);
	xfree(sendBuffer);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* commScheme_new_test */

extern bool
commScheme_del_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	commScheme_t schemeSend;
	commScheme_t schemeRecv;
	int          *sendBuffer;
	int          *recvBuffer;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	schemeSend = local_getFakeSendScheme(&sendBuffer);
	schemeRecv = local_getFakeRecvScheme(&recvBuffer);
	commScheme_del(&schemeRecv);
	if (schemeRecv != NULL)
		hasPassed = false;
	commScheme_del(&schemeSend);
	if (schemeSend != NULL)
		hasPassed = false;
	xfree(recvBuffer);
	xfree(sendBuffer);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* commScheme_del_test */

extern bool
commScheme_addBuffer_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	commScheme_t schemeSend;
	int          *sendBuffer;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	schemeSend = local_getFakeSendScheme(&sendBuffer);
	if (varArr_getLength(schemeSend->buffers) != 1)
		hasPassed = false;
	commScheme_del(&schemeSend);
	xfree(sendBuffer);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
commScheme_execute_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	commScheme_t schemeSend;
	commScheme_t schemeRecv;
	int          *sendBuffer;
	int          *recvBuffer;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	schemeSend = local_getFakeSendScheme(&sendBuffer);
	schemeRecv = local_getFakeRecvScheme(&recvBuffer);

	commScheme_execute(schemeRecv);
	commScheme_execute(schemeSend);
	commScheme_wait(schemeSend);
	commScheme_wait(schemeRecv);
	if (recvBuffer[0] != (rank + schemeRecv->size - 1) % schemeRecv->size)
		hasPassed = false;

	commScheme_del(&schemeRecv);
	commScheme_del(&schemeSend);
	xfree(recvBuffer);
	xfree(sendBuffer);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* commScheme_execute_test */

extern bool
commScheme_executeBlock_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	commScheme_t schemeSend;
	commScheme_t schemeRecv;
	int          *sendBuffer;
	int          *recvBuffer;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	schemeSend = local_getFakeSendScheme(&sendBuffer);
	schemeRecv = local_getFakeRecvScheme(&recvBuffer);

	commScheme_execute(schemeRecv);
	commScheme_executeBlock(schemeSend);
	commScheme_wait(schemeRecv);
	if (recvBuffer[0] != (rank + schemeRecv->size - 1) % schemeRecv->size)
		hasPassed = false;

	commScheme_del(&schemeRecv);
	commScheme_del(&schemeSend);
	xfree(recvBuffer);
	xfree(sendBuffer);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* commScheme_executeBlock_test */

extern bool
commScheme_wait_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	commScheme_t schemeSend;
	commScheme_t schemeRecv;
	int          *sendBuffer;
	int          *recvBuffer;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	schemeSend = local_getFakeSendScheme(&sendBuffer);
	schemeRecv = local_getFakeRecvScheme(&recvBuffer);

	commScheme_execute(schemeRecv);
	commScheme_execute(schemeSend);
	commScheme_wait(schemeSend);
	commScheme_wait(schemeRecv);
	if (recvBuffer[0] != (rank + schemeRecv->size - 1) % schemeRecv->size)
		hasPassed = false;

	commScheme_del(&schemeRecv);
	commScheme_del(&schemeSend);
	xfree(recvBuffer);
	xfree(sendBuffer);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* commScheme_wait_test */

/*--- Implementations of local functions --------------------------------*/
static commScheme_t
local_getFakeSendScheme(int **buffers)
{
	commScheme_t       scheme;
	commSchemeBuffer_t buf;
	int                to;

	scheme        = commScheme_new(MPI_COMM_WORLD, COMMSCHEME_TYPE_SEND, 0);
	*buffers      = xmalloc(sizeof(int) * 1);
	(*buffers)[0] = scheme->rank;
	to            = scheme->rank + 1;
	to            = (to >= scheme->size) ? 0 : to;
	buf           = commSchemeBuffer_new(*buffers, 1, MPI_INT, to);
	commScheme_addBuffer(scheme, buf);

	return scheme;
}

static commScheme_t
local_getFakeRecvScheme(int **buffers)
{
	commScheme_t       scheme;
	commSchemeBuffer_t buf;
	int                from;

	scheme   = commScheme_new(MPI_COMM_WORLD, COMMSCHEME_TYPE_RECV, 0);
	*buffers = xmalloc(sizeof(int) * 1);
	(*buffers)[0] = -1;
	from     = scheme->rank - 1;
	from     = (from < 0) ? scheme->size - 1 : from;
	buf      = commSchemeBuffer_new(*buffers, 1, MPI_INT, from);
	commScheme_addBuffer(scheme, buf);

	return scheme;
}
