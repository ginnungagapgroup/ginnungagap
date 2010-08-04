// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGET_ADT_H
#define GADGET_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "gadget.h"


/*--- ADT implementation ------------------------------------------------*/
struct gadget_struct {
	int            numFiles;
	char           **fileNames;
	FILE           **f;
	gadgetMode_t   mode;
	gadgetHeader_t *headers;
};

#endif
