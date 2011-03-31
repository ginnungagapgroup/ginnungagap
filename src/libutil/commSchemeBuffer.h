// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEMEBUFFER_H
#define COMMSCHEMEBUFFER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commSchemeBuffer.h
 * @ingroup libutilParallel
 * @brief  Provides a communication buffer interface (only with MPI).
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#ifdef WITH_MPI
#  include <mpi.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct commSchemeBuffer_struct *commSchemeBuffer_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern commSchemeBuffer_t
commSchemeBuffer_new(void *buf, int count, MPI_Datatype datatype, int rank);

extern void
commSchemeBuffer_del(commSchemeBuffer_t *buf);

extern void *
commSchemeBuffer_getBuf(commSchemeBuffer_t buf);

extern int
commSchemeBuffer_getCount(commSchemeBuffer_t buf);

extern MPI_Datatype
commSchemeBuffer_getDatatype(commSchemeBuffer_t buf);

extern int
commSchemeBuffer_getRank(commSchemeBuffer_t buf);

#endif
#endif
