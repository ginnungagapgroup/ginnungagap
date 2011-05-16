// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERSILO_ADT_H
#define GRIDREADERSILO_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#ifdef WITH_SILO
#  include "gridReader_adt.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridReaderSilo_struct {
	GRIDREADER_T_CONTENT
	char          *varName;
	dataVarType_t varType;
	int           varNumComponents;
};


#endif /* WITH_SILO */
#endif
