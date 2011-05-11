// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAPARTICLE_H
#define DATAPARTICLE_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include <stdbool.h>
#include "dataVar.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct dataParticle_struct *dataParticle_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern dataParticle_t
dataParticle_new(const char *dataParticleName,
                 int        dataParticleID,
                 int        hintNumVars);

extern dataParticle_t
dataParticle_getRef(dataParticle_t desc);

extern void
dataParticle_del(dataParticle_t *desc);

extern int
dataParticle_addVar(dataParticle_t desc, dataVar_t var);


/**
 * This will lock the particle description, i.e. no further things can
 * be changed and the particle thus described is considered to be
 * complete.
 */
extern void
dataParticle_lock(dataParticle_t desc);


/**
 * Check whether the particle description is complete (i.e. locked).
 */
extern bool
dataParticle_isLocked(const dataParticle_t desc);

extern char *
dataParticle_getNameHandle(const dataParticle_t desc);

extern int
dataParticle_getPartDescID(const dataParticle_t desc);

extern int
dataParticle_getNumVars(const dataParticle_t desc);

extern dataVar_t
dataParticle_getVarHandle(const dataParticle_t desc, int varNum);

#endif
