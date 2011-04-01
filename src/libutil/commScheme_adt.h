// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEME_ADT_H
#define COMMSCHEME_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commScheme_adt.h
 * @ingroup libutilParallel
 * @brief  Provides the main structure of the communication scheme ADT.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <mpi.h>
#include "varArr.h"


/*--- Local defines -----------------------------------------------------*/

/** @brief The scheme is before starting the communication. */
#define COMMSCHEME_STATUS_PREFIRE 0
/** @brief The scheme is doing the communication. */
#define COMMSCHEME_STATUS_FIRING 1
/** @brief The scheme has done the communication. */
#define COMMSCHEME_STATUS_POSTFIRE 2


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  The main communication scheme structure holding all required
 *         information.
 */
struct commScheme_struct {
	/** @brief The communicator used in this scheme. */
	MPI_Comm    comm;
	/** @brief The size of the communicator. */
	int         size;
	/** @brief The rank of this process in the communicator. */
	int         rank;
	/** @brief The tag used for sending and receiving. */
	int         tag;
	/** @brief The status of the communication scheme. */
	int         status;
	/** @brief Array holding the receive buffers. */
	varArr_t    buffersRecv;
	/** @brief Array holding the receive requests. */
	MPI_Request *requestsRecv;
	/** @brief Array holding the send buffers. */
	varArr_t    buffersSend;
	/** @brief Array holding the send requests. */
	MPI_Request *requestsSend;
};


#endif
