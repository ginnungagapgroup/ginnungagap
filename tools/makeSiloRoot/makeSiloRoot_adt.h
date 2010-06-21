// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKESILOROOT_ADT_H
#define MAKESILOROOT_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "makeSiloRootConfig.h"


/*--- Implemention of main structure ------------------------------------*/
struct makeSiloRoot_struct {
	int  numFiles;
	char **fileNames;
	char *outFileName;
};


#endif
