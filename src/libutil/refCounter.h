// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef REFCOUNTER_H
#define REFCOUNTER_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Exported types ----------------------------------------------------*/
typedef int refCounter_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern void
refCounter_init(refCounter_t *refCount);

extern void
refCounter_ref(refCounter_t *curRef);

extern bool
refCounter_deref(refCounter_t *curRef);

extern bool
refCounter_noReferenceLeft(refCounter_t curRef);


#endif
