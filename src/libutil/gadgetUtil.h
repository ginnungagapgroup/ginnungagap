// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETUTIL_H
#define GADGETUTIL_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>
#include <stdint.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern void
gadgetUtil_writeBlock(FILE *f, char *blockName, uint32_t dataBlockSize);

#endif
