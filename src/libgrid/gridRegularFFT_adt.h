// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARFFT_ADT_H
#define GRIDREGULARFFT_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridRegularFFT_struct {
	gridVarType_t     varType;
	gridPatch_t       patch;
	void              *data;
	gridPointInt_t    nProcs;
#if (defined WITH_MPI)
	gridPointUint32_t globalDims[NDIM];
	gridPointUint32_t localIdxLo[NDIM];
	gridPointUint32_t localIdxHi[NDIM];
	gridPointInt_t    localDims[NDIM];
	int               localNumRealElements;
#endif
};


#endif
