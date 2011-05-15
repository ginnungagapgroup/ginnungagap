// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERSILO_ADT_H
#define GRIDREADERSILO_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_adt.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridReaderSilo_struct {
	GRIDREADER_T_CONTENT
	char *prefix;
	int dbType;
};


#endif
