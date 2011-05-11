// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARFFT_ADT_H
#define GRIDREGULARFFT_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridRegularFFT_struct {
	gridRegular_t        grid;
	gridRegularDistrib_t distrib;
	int                  idxFFTVar;
	dataVar_t            var;
	gridPatch_t          patch;
	gridPointInt_t       nProcs;
	gridRegular_t        gridFFTed;
	gridRegularDistrib_t distribFFTed;
	int                  idxFFTVarFFTed;
	dataVar_t            varFFTed;
	gridPatch_t          patchFFTed;
	double               norm;
#if (defined WITH_MPI)
	gridPointUint32_t    globalDims[NDIM];
	gridPointUint32_t    localIdxLo[NDIM];
	gridPointUint32_t    localIdxHi[NDIM];
	gridPointInt_t       localDims[NDIM];
	int                  localNumRealElements;
#endif
};


#endif
