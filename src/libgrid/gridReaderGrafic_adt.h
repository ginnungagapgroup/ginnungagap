// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERGRAFIC_ADT_H
#define GRIDREADERGRAFIC_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_adt.h"
#include "../libutil/grafic.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridReaderGrafic_struct {
	GRIDREADER_T_CONTENT
	grafic_t grafic;
};


#endif
