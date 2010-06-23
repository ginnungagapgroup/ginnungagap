// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef SILOROOT_ADT_H
#define SILOROOT_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "makeSiloRootConfig.h"
#include <stdio.h>
#include <stdbool.h>
#include "../../src/libutil/varArr.h"


/*--- ADT implementation ------------------------------------------------*/
struct siloRoot_struct {
	char     *outFileName;
	char     *meshMasterName;
	varArr_t meshNames;
	int      numVars;
	char     **varMasterNames;
	varArr_t *varNames;
	bool     initialized;
	FILE     *logOut;
};


#endif
