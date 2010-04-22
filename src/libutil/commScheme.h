// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COMMSCHEME_H
#define COMMSCHEME_H


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
commScheme_new(MPI_Comm comm, int type, int tag);

extern void
commScheme_del(commScheme_t *scheme);

extern int
commScheme_addBuffer(commScheme_t scheme, commSchemeBuffer_t buffer);

extern void
commScheme_execute(commScheme_t scheme);

extern void
commScheme_executeBlock(commScheme_t scheme);

extern void
commScheme_wait(commScheme_t);

#endif
#endif
