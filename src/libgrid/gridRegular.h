// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_H
#define GRIDREGULAR_H


/*--- Includes ----------------------------------------------------------*/
#include "grid_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "gridSlab.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridRegular_struct *gridRegular_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridRegular_t
gridRegular_newWithoutData(uint32_t       ndims,
                           const uint32_t *dims,
                           const char     *gridName);


#ifdef WITH_SILO
extern void
gridRegular_writeSilo(gridRegular_t grid, const char *siloBaseName);

#endif

extern void
gridRegular_del(gridRegular_t *grid);


#endif
