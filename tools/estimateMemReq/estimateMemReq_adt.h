// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ESTIMATEMEMREQ_ADT_H
#define ESTIMATEMEMREQ_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "estimateMemReqConfig.h"


/*--- Implemention of main structure ------------------------------------*/
struct estimateMemReq_struct {
	int    dim1D;
	size_t dims[3];
	int    npTot;
	int    npX;
	int    npY;
	int    npZ;
	int    bytesPerCell;
	size_t memPerProcInBytes;
};


#endif
