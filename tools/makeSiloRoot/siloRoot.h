// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef SILOROOT_H
#define SILOROOT_H


/*--- Includes ----------------------------------------------------------*/
#include "makeSiloRootConfig.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct siloRoot_struct *siloRoot_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern siloRoot_t
siloRoot_new(const char *outFileName);

extern void
siloRoot_del(siloRoot_t *siloRoot);

extern void
siloRoot_write(siloRoot_t siloRoot);

extern void
siloRoot_parseFile(siloRoot_t siloRoot, const char *siloFileName);


#endif
