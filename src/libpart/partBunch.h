// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARTBUNCH_H
#define PARTBUNCH_H


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include "partDesc.h"
#include <stdint.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct partBunch_struct *partBunch_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern partBunch_t
partBunch_new(partDesc_t desc, uint64_t initialNumParts);

extern void
partBunch_del(partBunch_t *bunch);

extern void
partBunch_allocMem(partBunch_t bunch);

extern void
partBunch_freeMem(partBunch_t bunch);

extern void
partBunch_resize(partBunch_t bunch, uint64_t numParticles);

extern bool
partBunch_isAllocated(const partBunch_t bunch);

extern uint64_t
partBunch_getNumParticles(const partBunch_t bunch);

extern void
partBunch_setNumParticles(partBunch_t bunch, uint64_t numParticles);


#endif
