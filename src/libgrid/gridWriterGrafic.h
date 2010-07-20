// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERGRAFIC_H
#define GRIDWRITERGRAFIC_H


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
typedef struct gridWriterGrafic_struct *gridWriterGrafic_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridWriterGrafic_t
gridWriterGrafic_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridWriterGrafic_del(gridWriter_t *writer);

extern void
gridWriterGrafic_activate(gridWriter_t writer);

extern void
gridWriterGrafic_deactivate(gridWriter_t writer);

extern void
gridWriterGrafic_writeGridPatch(gridWriter_t   writer,
                                gridPatch_t    patch,
                                const char     *patchName,
                                gridPointDbl_t origin,
                                gridPointDbl_t delta);

extern void
gridWriterGrafic_writeGridRegular(gridWriter_t  writer,
                                  gridRegular_t grid);


#ifdef WITH_MPI
extern void
gridWriterGrafic_initParallel(gridWriter_t writer, MPI_Comm mpiComm);

#endif

#endif
