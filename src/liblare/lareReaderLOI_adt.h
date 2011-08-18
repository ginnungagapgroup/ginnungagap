// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LAREREADERLOI_ADT_H
#define LAREREADERLOI_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReader_adt.h"


/*--- ADT implementation ------------------------------------------------*/
struct lareReaderLOI_struct {
	LAREREADER_T_CONTENT
	char     *fileName;
	uint32_t ngrid;
};


#endif
