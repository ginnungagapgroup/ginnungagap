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
#ifdef WITH_MPI
#  include <mpi.h>
#  include "varArr.h"


/*--- Local defines -----------------------------------------------------*/
#  define COMMSCHEME_STATUS_PREFIRE  0
#  define COMMSCHEME_STATUS_FIRING   1
#  define COMMSCHEME_STATUS_POSTFIRE 2


/*--- ADT implementation ------------------------------------------------*/
struct commScheme_struct {
	MPI_Comm    comm;
	int         size;
	int         rank;
	int         tag;
	int         status;
	varArr_t    buffersRecv;
	MPI_Request *requestsRecv;
	varArr_t    buffersSend;
	MPI_Request *requestsSend;
};


#endif
#endif
