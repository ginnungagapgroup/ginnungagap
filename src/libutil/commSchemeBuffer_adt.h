// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEMEBUFFER_ADT_H
#define COMMSCHEMEBUFFER_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commSchemeBuffer_adt.h
 * @ingroup libutilParallel
 * @brief  Provides the implementation of the buffer structure for the
 *         communication scheme.
 */


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
