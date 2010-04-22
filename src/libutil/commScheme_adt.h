// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEME_ADT_H
#define COMMSCHEME_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#ifdef WITH_MPI
#  include <mpi.h>
#  include "varArr.h"


/*--- ADT implementation ------------------------------------------------*/
struct commScheme_struct {
	MPI_Comm    comm;
	int         size;
	int         rank;
	int         type;
	int         tag;
	int         status;
	varArr_t    buffers;
	MPI_Request *requests;
};


#endif
#endif
