// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commSchemeBuffer.c
 * @ingroup libutilParallel
 * @brief  Provides the implementation of the communication buffer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "commSchemeBuffer.h"
#include <mpi.h>
#include <assert.h>
#include "xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "commSchemeBuffer_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern commSchemeBuffer_t
commSchemeBuffer_new(void *buf, int count, MPI_Datatype datatype, int rank)
{
	commSchemeBuffer_t buffer;

	assert(buf != NULL);
	assert(count >= 0);
	assert(rank >= 0);

	buffer           = xmalloc(sizeof(struct commSchemeBuffer_struct));
	buffer->buf      = buf;
	buffer->count    = count;
	buffer->datatype = datatype;
	buffer->rank     = rank;

	return buffer;
}

extern void
commSchemeBuffer_del(commSchemeBuffer_t *buf)
{
	assert(buf != NULL && *buf != NULL);

	xfree(*buf);

	*buf = NULL;
}

extern void *
commSchemeBuffer_getBuf(const commSchemeBuffer_t buf)
{
	assert(buf != NULL);
	return buf->buf;
}

extern int
commSchemeBuffer_getCount(const commSchemeBuffer_t buf)
{
	assert(buf != NULL);
	return buf->count;
}

extern MPI_Datatype
commSchemeBuffer_getDatatype(const commSchemeBuffer_t buf)
{
	assert(buf != NULL);
	return buf->datatype;
}

extern int
commSchemeBuffer_getRank(const commSchemeBuffer_t buf)
{
	assert(buf != NULL);
	return buf->rank;
}

/*--- Implementations of local functions --------------------------------*/
