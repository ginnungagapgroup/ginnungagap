// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERSILO_H
#define GRIDWRITERSILO_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#ifdef WITH_SILO
#  include "gridRegular.h"
#  include "gridPatch.h"
#  include "gridPoint.h"
#  ifdef WITH_MPI
#    include <mpi.h>
#  endif


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridWriterSilo_struct *gridWriterSilo_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridWriterSilo_t
gridWriterSilo_new(const char *prefix, int dbType);

extern void
gridWriterSilo_del(gridWriterSilo_t *writer);

extern void
gridWriterSilo_activate(gridWriterSilo_t writer);

extern void
gridWriterSilo_deactivate(gridWriterSilo_t writer);

extern void
gridWriterSilo_writeGridPatch(gridWriterSilo_t writer,
                              gridPatch_t      patch,
                              const char       *patchName,
                              gridPointDbl_t   origin,
                              gridPointDbl_t   delta);

extern void
gridWriterSilo_writeGridRegular(gridWriterSilo_t writer,
                                gridRegular_t    grid);


#  ifdef WITH_MPI
extern void
gridWriterSilo_initParallel(gridWriterSilo_t writer,
                            int              numFiles,
                            MPI_Comm         mpiComm,
                            int              mpiTag);

#  endif

#endif
#endif
