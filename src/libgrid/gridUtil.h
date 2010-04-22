// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDUTIL_H
#define GRIDUTIL_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>
#include <stdint.h>

/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridUtil_intersection1D(uint32_t loA,
                       uint32_t hiA,
                       uint32_t loB,
                       uint32_t hiB,
                       uint32_t *loC,
                       uint32_t *hiC);


#endif
