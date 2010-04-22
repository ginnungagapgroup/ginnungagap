// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#ifdef WITH_MPI
#  include "commScheme.h"
#  include "commSchemeBuffer.h"
#  include <mpi.h>
#  include <assert.h>
#  include "xmem.h"
#  include "varArr.h"


/*--- Implemention of main structure ------------------------------------*/
#  include "commScheme_adt.h"
#  include "commSchemeBuffer_adt.h"


/*--- Local defines -----------------------------------------------------*/
#  define COMMSCHEME_STATUS_PREFIRE  0
#  define COMMSCHEME_STATUS_FIRING   1
#  define COMMSCHEME_STATUS_POSTFIRE 2


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern commScheme_t
commScheme_new(MPI_Comm comm, int type, int tag)
{
	commScheme_t scheme;

	assert(comm != MPI_COMM_NULL);
	assert(type == COMMSCHEME_TYPE_SEND || type == COMMSCHEME_TYPE_RECV);
	assert(tag >= 0 && tag < 32767);

	scheme = xmalloc(sizeof(struct commScheme_struct));
	scheme->comm = comm;
	MPI_Comm_rank(scheme->comm, &(scheme->rank));
	MPI_Comm_size(scheme->comm, &(scheme->size));
	scheme->type     = type;
	scheme->tag      = tag;
	scheme->buffers  = varArr_new(scheme->size / 10);
	scheme->status   = COMMSCHEME_STATUS_PREFIRE;
	scheme->requests = NULL;

	return scheme;
}

extern void
commScheme_del(commScheme_t *scheme)
{
	assert(scheme != NULL && *scheme != NULL);

	if ((*scheme)->status == COMMSCHEME_STATUS_FIRING)
		commScheme_wait(*scheme);

	while (varArr_getLength((*scheme)->buffers) != 0) {
		commSchemeBuffer_t buf = varArr_remove((*scheme)->buffers, 0);
		commSchemeBuffer_del(&buf);
	}
	varArr_del(&((*scheme)->buffers));
	xfree(*scheme);

	*scheme = NULL;
}

extern int
commScheme_addBuffer(commScheme_t scheme, commSchemeBuffer_t buffer)
{
	assert(scheme != NULL);
	assert(buffer != NULL);
	assert(scheme->status == COMMSCHEME_STATUS_PREFIRE);

	return varArr_insert(scheme->buffers, buffer);
}

extern void
commScheme_execute(commScheme_t scheme)
{
	int numBuffers;

	assert(scheme != NULL);
	assert(scheme->status == COMMSCHEME_STATUS_PREFIRE);

	numBuffers = varArr_getLength(scheme->buffers);
	if (numBuffers <= 0)
		return;

	scheme->requests = xmalloc(sizeof(MPI_Request) * numBuffers);

	if (scheme->type == COMMSCHEME_TYPE_SEND) {
		for (int i = 0; i < numBuffers; i++) {
			commSchemeBuffer_t buf;
			buf = varArr_getElementHandle(scheme->buffers, i);
			MPI_Isend(buf->buf, buf->count, buf->datatype, buf->rank,
			          scheme->tag, scheme->comm, scheme->requests + i);
		}
	} else {
		for (int i = 0; i < numBuffers; i++) {
			commSchemeBuffer_t buf;
			buf = varArr_getElementHandle(scheme->buffers, i);
			MPI_Irecv(buf->buf, buf->count, buf->datatype, buf->rank,
			          scheme->tag, scheme->comm, scheme->requests + i);
		}
	}
	scheme->status = COMMSCHEME_STATUS_FIRING;
}

extern void
commScheme_executeBlock(commScheme_t scheme)
{
	assert(scheme != NULL);
	assert(scheme->status == COMMSCHEME_STATUS_PREFIRE);

	commScheme_execute(scheme);
	commScheme_wait(scheme);
}

extern void
commScheme_wait(commScheme_t scheme)
{
	int numRequests;

	assert(scheme != NULL);

	if (scheme->status != COMMSCHEME_STATUS_FIRING)
		return;

	numRequests = varArr_getLength(scheme->buffers);
	MPI_Waitall(numRequests, scheme->requests, MPI_STATUSES_IGNORE);

	xfree(scheme->requests);
	scheme->requests = NULL;
	scheme->status   = COMMSCHEME_STATUS_POSTFIRE;
}

/*--- Implementations of local functions --------------------------------*/
#endif
