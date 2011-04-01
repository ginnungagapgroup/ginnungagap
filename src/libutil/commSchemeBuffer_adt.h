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
#include <mpi.h>


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure holding the buffer information.  */
struct commSchemeBuffer_struct {
	/** @brief The actual buffer array. */
	void         *buf;
	/** @brief The number of elements in the buffer. */
	int          count;
	/** @brief The MPI datatype of the elements in the buffer. */
	MPI_Datatype datatype;
	/** @brief The associated rank. */
	int          rank;
};


#endif
