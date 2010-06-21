// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKESILOROOT_H
#define MAKESILOROOT_H


/*--- Includes ----------------------------------------------------------*/
#include "makeSiloRootConfig.h"
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct makeSiloRoot_struct *makeSiloRoot_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern makeSiloRoot_t
makeSiloRoot_new(const char *siloFileStem,
                 const char *siloFileSuffix,
                 const char *siloOutputFileName,
                 bool       force);

extern void
makeSiloRoot_run(makeSiloRoot_t msr);

extern void
makeSiloRoot_del(makeSiloRoot_t *msr);


#endif
