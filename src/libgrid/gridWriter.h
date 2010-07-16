// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITER_H
#define GRIDWRITER_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridWriter_struct *gridWriter_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridWriter_t
gridWriter_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridWriter_del(gridWriter_t *writer);

extern void
gridWriter_activate(gridWriter_t writer);

extern void
gridWriter_deactivate(gridWriter_t writer);

extern void
gridWriter_writeGridPatch(gridWriter_t   writer,
                          gridPatch_t    patch,
                          const char     *patchName,
                          gridPointDbl_t origin,
                          gridPointDbl_t delta);

extern void
gridWriter_writeGridRegular(gridWriter_t  writer,
                            gridRegular_t grid);


#ifdef WITH_MPI
extern void
gridWriter_initParallel(gridWriter_t writer, MPI_Comm mpiComm);

#endif


#endif
