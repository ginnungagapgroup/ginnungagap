// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETHEADER_ADT_H
#define GADGETHEADER_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Definess ----------------------------------------------------------*/
#define GADGETHEADER_SIZE      256
#define GADGETHEADER_SIZE_USED 196
#define GADGETHEADER_SIZE_UNUSED (256 - 196)

/*--- ADT implementation ------------------------------------------------*/
struct gadgetHeader_struct {
	uint32_t np[6];
	double   massarr[6];
	double   expansion;
	double   redshift;
	int32_t  flagsfr;
	int32_t  flagfeedback;
	uint32_t nall[6];
	int32_t  flagcooling;
	int32_t  numfiles;
	double   boxsize;
	double   omega0;
	double   omegalambda;
	double   hubbleparameter;
	int32_t  flagstellarage;
	int32_t  flagmetal;
	uint32_t nallhighw[6];
	int32_t  flagentropy;
};

#endif
