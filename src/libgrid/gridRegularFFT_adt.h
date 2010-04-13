// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARFFT_ADT_H
#define GRIDREGULARFFT_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridRegularFFT_struct {
	gridVarType_t varType;
	int           direction;
	void          *remap[NDIM];
	void          *remapParams[NDIM];
	void          *fft1d[NDIM];
	void          *fftPlans[NDIM];
};


#endif
