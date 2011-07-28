// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commScheme.c
 * @ingroup libutilParallelCommScheme
 * @brief  Provides the implementation of the communication scheme
 *         interface.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "commScheme.h"
#include "commSchemeBuffer.h"
#include <mpi.h>
#include <assert.h>
#include "xmem.h"
#include "varArr.h"


/*--- Implementation of main structure ----------------------------------*/
#include "commScheme_adt.h"
#include "commSchemeBuffer_adt.h"


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Starts the receiving.
 *
 * @param[in,out]  scheme
 *                    The communication scheme for which to start the
 *                    receiving process.
 *
 * @return  Returns nothing.
 */
inline static void
local_startReceiving(commScheme_t scheme);


/**
 * @brief  Starts the sending.
 *
 * @param[in,out]  scheme
 *                    The communication scheme for which to start the
 *                    sending process.
 *
 * @return  Returns nothing.
 */
inline static void
local_startSending(commScheme_t scheme);


/*--- Implementations of exported functions -----------------------------*/
extern commScheme_t
commScheme_new(MPI_Comm comm, int tag)
{
	commScheme_t scheme;

	assert(comm != MPI_COMM_NULL);
	assert(tag >= 0 && tag < 32767);

	scheme       = xmalloc(sizeof(struct commScheme_struct));
	scheme->comm = comm;
	MPI_Comm_rank(scheme->comm, &(scheme->rank));
	MPI_Comm_size(scheme->comm, &(scheme->size));
	scheme->tag          = tag;
	scheme->buffersRecv  = varArr_new(scheme->size / 10);
	scheme->requestsRecv = NULL;
	scheme->buffersSend  = varArr_new(scheme->size / 10);
	scheme->requestsSend = NULL;
	scheme->status       = COMMSCHEME_STATUS_PREFIRE;

	return scheme;
}

extern void
commScheme_del(commScheme_t *scheme)
{
	assert(scheme != NULL && *scheme != NULL);

	if ((*scheme)->status == COMMSCHEME_STATUS_FIRING)
		commScheme_wait(*scheme);

	while (varArr_getLength((*scheme)->buffersRecv) != 0) {
		commSchemeBuffer_t buf = varArr_remove((*scheme)->buffersRecv, 0);
		commSchemeBuffer_del(&buf);
	}
	varArr_del(&((*scheme)->buffersRecv));
	while (varArr_getLength((*scheme)->buffersSend) != 0) {
		commSchemeBuffer_t buf = varArr_remove((*scheme)->buffersSend, 0);
		commSchemeBuffer_del(&buf);
	}
	varArr_del(&((*scheme)->buffersSend));
	xfree(*scheme);

	*scheme = NULL;
}

extern int
commScheme_addBuffer(commScheme_t       scheme,
                     commSchemeBuffer_t buffer,
                     int                type)
{
	assert(scheme != NULL);
	assert(buffer != NULL);
	assert(type == COMMSCHEME_TYPE_SEND || type == COMMSCHEME_TYPE_RECV);
	assert(scheme->status == COMMSCHEME_STATUS_PREFIRE);

	if (type == COMMSCHEME_TYPE_SEND)
		return varArr_insert(scheme->buffersSend, buffer);

	return varArr_insert(scheme->buffersRecv, buffer);
}

extern void
commScheme_fire(commScheme_t scheme)
{
	assert(scheme != NULL);
	assert(scheme->status == COMMSCHEME_STATUS_PREFIRE);

	local_startReceiving(scheme);
	local_startSending(scheme);

	scheme->status = COMMSCHEME_STATUS_FIRING;
}

extern void
commScheme_fireBlock(commScheme_t scheme)
{
	assert(scheme != NULL);
	assert(scheme->status == COMMSCHEME_STATUS_PREFIRE);

	commScheme_fire(scheme);
	commScheme_wait(scheme);
}

extern void
commScheme_wait(commScheme_t scheme)
{
	int numRequests;

	assert(scheme != NULL);

	if (scheme->status != COMMSCHEME_STATUS_FIRING)
		return;

	numRequests = varArr_getLength(scheme->buffersSend);
	if (numRequests > 0)
		MPI_Waitall(numRequests, scheme->requestsSend, MPI_STATUSES_IGNORE);
	xfree(scheme->requestsSend);
	scheme->requestsSend = NULL;

	numRequests          = varArr_getLength(scheme->buffersRecv);
	if (numRequests > 0)
		MPI_Waitall(numRequests, scheme->requestsRecv, MPI_STATUSES_IGNORE);
	xfree(scheme->requestsRecv);
	scheme->requestsRecv = NULL;

	scheme->status       = COMMSCHEME_STATUS_POSTFIRE;
}

/*--- Implementations of local functions --------------------------------*/
inline static void
local_startReceiving(commScheme_t scheme)
{
	int numBuffersRecv;

	numBuffersRecv       = varArr_getLength(scheme->buffersRecv);
	scheme->requestsRecv = xmalloc(sizeof(MPI_Request) * numBuffersRecv);

	for (int i = 0; i < numBuffersRecv; i++) {
		commSchemeBuffer_t buf;
		buf = varArr_getElementHandle(scheme->buffersRecv, i);
		MPI_Irecv(buf->buf, buf->count, buf->datatype, buf->rank,
		          scheme->tag, scheme->comm, scheme->requestsRecv + i);
	}
}

inline static void
local_startSending(commScheme_t scheme)
{
	int                firstSendBuf = 0;
	int                numBuffersSend;
	commSchemeBuffer_t buf;

	numBuffersSend       = varArr_getLength(scheme->buffersSend);
	scheme->requestsSend = xmalloc(sizeof(MPI_Request) * numBuffersSend);

	while (firstSendBuf < numBuffersSend) {
		buf = varArr_getElementHandle(scheme->buffersSend, firstSendBuf);
		if (buf->rank > scheme->rank)
			break;
		firstSendBuf++;
	}
	firstSendBuf %= numBuffersSend;

	for (int i = firstSendBuf; i < numBuffersSend; i++) {
		buf = varArr_getElementHandle(scheme->buffersSend, i);
		MPI_Isend(buf->buf, buf->count, buf->datatype, buf->rank,
		          scheme->tag, scheme->comm, scheme->requestsSend + i);
	}

	for (int i = 0; i < firstSendBuf; i++) {
		buf = varArr_getElementHandle(scheme->buffersSend, i);
		MPI_Isend(buf->buf, buf->count, buf->datatype, buf->rank,
		          scheme->tag, scheme->comm, scheme->requestsSend + i);
	}
}
