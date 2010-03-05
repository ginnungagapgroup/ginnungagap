// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_ADT_H
#define GRIDREGULAR_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridRegular_struct {
	char            *gridName;
	gridPointDbl_t  globalOrigin;
	gridPointDbl_t  globalExtent;
	gridPointSize_t globalDims;
	gridPointDbl_t  delta;
	uint64_t        globalNumCells;
	gridPointDbl_t  localOrigin;
	gridPointDbl_t  localExtent;
	gridPointSize_t localDims;
	gridPointSize_t localStart;
	uint64_t        localNumCells;
#ifdef WITH_MPI
	MPI_Comm        mpiCommGrid;
	int             mpiCommGridSize;
	int             mpiCommGridRank;
	MPI_Comm        mpiCommCart;
	int             mpiCommCartSize;
	int             mpiCommCartRank;
	gridPointInt_t  mpiCommCartCoords;
	gridPointInt_t  mpiNumProcs;
#endif
};


#endif
