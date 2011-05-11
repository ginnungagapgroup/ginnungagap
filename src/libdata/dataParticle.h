// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef PARTDESC_H
#define PARTDESC_H


/*--- Includes ----------------------------------------------------------*/
#include "partConfig.h"
#include <stdbool.h>
#include "../libgrid/gridVar.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct partDesc_struct *partDesc_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern partDesc_t
partDesc_new(const char *partDescName, int partDescID, int hintNumVars);

extern partDesc_t
partDesc_getRef(partDesc_t desc);

extern void
partDesc_del(partDesc_t *desc);

extern int
partDesc_addVar(partDesc_t desc, gridVar_t var);


/**
 * This will lock the particle description, i.e. no further things can
 * be changed and the particle thus described is considered to be
 * complete.
 */
extern void
partDesc_lock(partDesc_t desc);


/**
 * Check whether the particle description is complete (i.e. locked).
 */
extern bool
partDesc_isLocked(const partDesc_t desc);

extern char *
partDesc_getNameHandle(const partDesc_t desc);

extern int
partDesc_getPartDescID(const partDesc_t desc);

extern int
partDesc_getNumVars(const partDesc_t desc);

extern gridVar_t
partDesc_getVarHandle(const partDesc_t desc, int varNum);

#endif
