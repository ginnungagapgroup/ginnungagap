// Copyright (C) 2010, 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARTBUNCH_H
#define PARTBUNCH_H


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include <stdint.h>
#include <stdbool.h>
#include "../libdata/dataParticle.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct partBunch_struct *partBunch_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern partBunch_t
partBunch_new(dataParticle_t desc, uint64_t initialNumParts);

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


/**
 * @brief  Returns a pointer to the requested component at a requested
 *         position.
 */
extern void *
partBunch_at(const partBunch_t bunch, const int idxOfVar, const uint64_t i);


#endif
