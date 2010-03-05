// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef SILOTOOLS_H
#define SILOTOOLS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern char *
siloTools_getFileName(const char *siloBaseName, int rank, int size);

extern char *
siloTools_getMasterFileName(const char *siloBaseName);

extern char *
siloTools_getGridName(const char *gridName, int rank, int size);

extern char *
siloTools_prependFileName(char *gridName, const char *siloFileName);

#endif
