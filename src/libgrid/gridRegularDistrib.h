// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARDISTRIB_H
#define GRIDREGULARDISTRIB_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular.h"
#include "gridPatch.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridRegularDistrib_struct *gridRegularDistrib_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridRegularDistrib_t
gridRegularDistrib_new(gridRegular_t grid, gridPointInt_t nProcs);


extern void
gridRegularDistrib_del(gridRegularDistrib_t *distrib);


#ifdef WITH_MPI
extern void
gridRegularDistrib_initMPI(gridRegularDistrib_t distrib,
                           gridPointInt_t       nProcs,
                           MPI_Comm             comm);

extern int
gridRegularDistrib_getLocalRank(gridRegularDistrib_t distrib);

#endif

extern gridPatch_t
gridRegularDistrib_getPatchForRank(gridRegularDistrib_t distrib, int rank);


#endif
