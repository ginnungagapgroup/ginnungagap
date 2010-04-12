// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARDISTRIB_ADT_H
#define GRIDREGULARDISTRIB_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "../libutil/refCounter.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridRegularDistrib_struct {
	refCounter_t   refCounter;
	gridRegular_t  grid;
	gridPointInt_t nProcs;
	int            numProcs;
#ifdef WITH_MPI
	MPI_Comm       commGlobal;
	MPI_Comm       commCart;
#endif
};


#endif
