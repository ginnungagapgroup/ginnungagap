// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERSILO_H
#define GRIDWRITERSILO_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include "gridRegular.h"
#include "gridPatch.h"
#include "gridPoint.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridWriterSilo_struct *gridWriterSilo_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridWriterSilo_t
gridWriterSilo_new(const char *prefix, int dbType);

extern gridWriterSilo_t
gridWriterSilo_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridWriterSilo_del(gridWriter_t *writer);

extern void
gridWriterSilo_activate(gridWriter_t writer);

extern void
gridWriterSilo_deactivate(gridWriter_t writer);

extern void
gridWriterSilo_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta);

extern void
gridWriterSilo_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid);


#ifdef WITH_MPI
extern void
gridWriterSilo_initParallel(gridWriter_t writer, MPI_Comm mpiComm);

#endif

#endif
