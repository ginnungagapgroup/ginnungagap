// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_H
#define GRIDREGULAR_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "gridVar.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridRegular_struct *gridRegular_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridRegular_t
gridRegular_newWithoutData(const char      *gridName,
                           gridPointSize_t dims);

extern void
gridRegular_del(gridRegular_t *grid);

extern void
gridRegular_setOrigin(gridRegular_t grid, gridPointDbl_t origin);

extern void
gridRegular_setExtent(gridRegular_t grid, gridPointDbl_t extent);

extern int
gridRegular_attachVar(gridRegular_t grid, gridVar_t var);

extern void
gridRegular_allocVar(gridRegular_t grid, int varToAlloc);

extern void
gridRegular_deallocVar(gridRegular_t grid, int varToDealloc);

#ifdef WITH_MPI
extern void
gridRegular_mpiSetDistribution(gridRegular_t  grid,
                               gridPointInt_t nProcs,
                               MPI_Comm       mpiCommGrid);

#endif


#ifdef WITH_SILO
extern void
gridRegular_writeSilo(gridRegular_t grid, const char *siloBaseName);

#endif


#endif
