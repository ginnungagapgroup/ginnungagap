// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEMEBUFFER_ADT_H
#define COMMSCHEMEBUFFER_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#ifdef WITH_MPI
#include <mpi.h>


/*--- ADT implementation ------------------------------------------------*/
struct commSchemeBuffer_struct {
	void         *buf;
	int          count;
	MPI_Datatype datatype;
	int          rank;
};


#endif
#endif
