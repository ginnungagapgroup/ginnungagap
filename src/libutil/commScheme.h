// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEME_H
#define COMMSCHEME_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/commScheme.h
 * @ingroup libutilParallel
 * @brief  Provides a communication scheme interface (only available
 *         with MPI).
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#ifdef WITH_MPI
#  include "commSchemeBuffer.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct commScheme_struct *commScheme_t;


/*--- Exported defines --------------------------------------------------*/
#  define COMMSCHEME_TYPE_SEND 0
#  define COMMSCHEME_TYPE_RECV 1

/*--- Prototypes of exported functions ----------------------------------*/
extern commScheme_t
commScheme_new(MPI_Comm comm, int tag);

extern void
commScheme_del(commScheme_t *scheme);

extern int
commScheme_addBuffer(commScheme_t       scheme,
                     commSchemeBuffer_t buffer,
                     int                type);

extern void
commScheme_fire(commScheme_t scheme);

extern void
commScheme_fireBlock(commScheme_t scheme);

extern void
commScheme_wait(commScheme_t);

#endif
#endif
