// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERBOV_ADT_H
#define GRIDREADERBOV_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader_adt.h"
#include "../libutil/bov.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridReaderBov_struct {
	GRIDREADER_T_CONTENT;
	bov_t bov;
};


#endif
