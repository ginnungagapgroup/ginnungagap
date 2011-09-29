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

/**
 * @name  Creating and Deleting
 *
 * @{
 */

/**
 * @brief  Creates a new empty HDF5 writer.
 *
 * @return  The new writer.
 */
extern gridWriterHDF5_t
gridWriterHDF5_new(void);


/** @copydoc gridWriter_newFromIni() */
extern gridWriterHDF5_t
gridWriterHDF5_newFromIni(parse_ini_t ini, const char *sectionName);


/** @copydoc gridWriter_del() */
extern void
gridWriterHDF5_del(gridWriter_t *writer);


/** @} */

/**
 * @name  Setting
 * @{
 */

extern void
gridWriterHDF5_setFileName(gridWriterHDF5_t w, const char *fileName);

extern void
gridWriterHDF5_setDoChunking(gridWriterHDF5_t w, bool doChunking);

extern void
gridWriterHDF5_setChunkSize(gridWriterHDF5_t w, gridPointUint32_t chunkSize);

extern void
gridWriterHDF5_setDoChecksum(gridWriterHDF5_t w, bool doChecksum);

extern void
gridWriterHDF5_setDoCompression(gridWriterHDF5_t w, bool doCompression);

extern void
gridWriterHDF5_setCompressionFilter(gridWriterHDF5_t w,
                                    const char *filterName);

/** @} */

/**
 * @name  Using
 *
 * @{
 */

/** @copydoc gridWriter_activate() */
extern void
gridWriterHDF5_activate(gridWriter_t writer);


/** @copydoc gridWriter_deactivate() */
extern void
gridWriterHDF5_deactivate(gridWriter_t writer);


/** @copydoc gridWriter_writeGridPatch() */
extern void
gridWriterHDF5_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta);


/** @copydoc gridWriter_writeGridRegular() */
extern void
gridWriterHDF5_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid);


/** @} */

#ifdef WITH_MPI

/**
 * @name  Additional Initialization
 *
 * @{
 */

/** @copydoc gridWriter_initParallel() */
extern void
gridWriterHDF5_initParallel(gridWriter_t writer, MPI_Comm mpiComm);


/** @} */

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
