// Copyright (C) 2011, 2012, Steffen Knollmann
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
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT handle --------------------------------------------------------*/

/** @brief  The handle for the writer object. */
typedef struct gridWriterHDF5_struct *gridWriterHDF5_t;


/*--- Prototypes of implemented abstract functions ----------------------*/

/**
 * @name  Creating and Deleting (Public, Virtual)
 *
 * @{
 */

/** @copydoc gridWriter_del() */
extern void
gridWriterHDF5_del(gridWriter_t *writer);


/** @} */


/**
 * @name  Using (Public, Virtual)
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
 * @name  Additional Initialization (Public, Virtual)
 *
 * @{
 */

/** @copydoc gridWriter_initParallel() */
extern void
gridWriterHDF5_initParallel(gridWriter_t writer, MPI_Comm mpiComm);


/** @} */

#endif


/*--- Prototypes of final functions -------------------------------------*/

/**
 * @name  Creating and Deleting (Public, Final)
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


/** @} */


/**
 * @name  Setting (Public, Final)
 *
 * @{
 */

/**
 * @brief  This will write the data in a chunked manner instead of
 *         contiguous.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  doChunking
 *                Toggles between chunked and contiguous writing.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setDoChunking(gridWriterHDF5_t w, bool doChunking);


/**
 * @brief  This will set the size of the chunks that are written.
 *
 * Calling this function will also activate the chunked writing.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  chunkSize
 *                The size of the chunk.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setChunkSize(gridWriterHDF5_t w, gridPointUint32_t chunkSize);


/**
 * @brief  This will activate the checksum calculation.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  doChecksum
 *                Toggles between checksumming and no checksumming.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setDoChecksum(gridWriterHDF5_t w, bool doChecksum);


/**
 * @brief  This will activate the compression of the data.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  doCompression
 *                Toggles between compression and no compression.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setDoCompression(gridWriterHDF5_t w, bool doCompression);


/**
 * @brief  This will set the compression filter that should be used.
 *
 * Setting the compression filter also activates the compression.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  *filterName
 *                The name of the filter that should be used.  Supported
 *                filters are @c gzip and @c szip (depending on the
 *                capabilities of the HDF5 library).
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setCompressionFilter(gridWriterHDF5_t w,
                                    const char       *filterName);
                                    

/**
 * @brief  This will activate writing only a specified patch to the ouput.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  doPatch
 *                Toggles between patch and whole grid.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setDoPatch(gridWriterHDF5_t w, bool doPatch);


/**
 * @brief  This will set the patch for the output instead of the whole grid.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  Lo
 *                Patch lower corner.
 * @param[in]  d
 *                Patch dimensions.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setRtw(gridWriterHDF5_t w, int32_t *Lo, gridPointUint32_t d);


/** @} */


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
