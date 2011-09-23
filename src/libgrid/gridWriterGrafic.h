// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERGRAFIC_H
#define GRIDWRITERGRAFIC_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterGrafic.h
 * @ingroup  libgridIOOutGrafic
 * @brief  Provides the interface for the Grafic writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include <stdbool.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridRegular.h"
#include "gridPatch.h"
#include "gridPoint.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridWriterGrafic_struct *gridWriterGrafic_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridWriterGrafic_t
gridWriterGrafic_new(const char *prefix, bool isWhiteNoise);

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


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOOutGrafic Grafic Writer
 * @ingroup libgridIOOut
 * @brief  Provides the Grafic Writer.
 * 
 * @section libgridIOOutGraficIniFormat  Expected Format for Ini Files
 *
 * @code
 * [SectionName]
 * @endcode
 */


#endif
