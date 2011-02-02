// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LAREREADERLEGACY_ADT_H
#define LAREREADERLEGACY_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReader_adt.h"


/*--- ADT implementation ------------------------------------------------*/
struct lareReaderLegacy_struct {
	LAREREADER_T_CONTENT
	char     *fileName;
	bool     hasHeader;
	uint32_t ngrid;
};


#endif
