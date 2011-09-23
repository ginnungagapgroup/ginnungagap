// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERHDF5_H
#define GRIDWRITERHDF5_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterHDF5.h
 * @ingroup  libgridIOOutHDF5
 * @brief  Provides the interface for the HDF5 writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridRegular.h"
#include "gridPatch.h"
#include "gridPoint.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the writer object. */
typedef struct gridWriterHDF5_struct *gridWriterHDF5_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridWriterHDF5_t
gridWriterHDF5_new(void);

extern gridWriterHDF5_t
gridWriterHDF5_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridWriterHDF5_del(gridWriter_t *writer);

extern void
gridWriterHDF5_activate(gridWriter_t writer);

extern void
gridWriterHDF5_deactivate(gridWriter_t writer);

extern void
gridWriterHDF5_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta);

extern void
gridWriterHDF5_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid);


#ifdef WITH_MPI
extern void
gridWriterHDF5_initParallel(gridWriter_t writer, MPI_Comm mpiComm);

#endif


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOOutHDF5 HDF5 Writer
 * @ingroup libgridIOOut
 * @brief  Provides the HDF5 Writer.
 *
 * @section libgridIOOutHDF5IniFormat  Expected Format for Ini Files
 *
 * @code
 * [SectionName]
 * @endcode
 */


#endif
