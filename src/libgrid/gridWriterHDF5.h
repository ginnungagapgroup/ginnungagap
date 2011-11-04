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

/**
 * @brief  This will set the file name of the output file.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  *fileName
 *                The file name of the output file.  The function will
 *                create a copy of the string, hence the calling process
 *                is still responsible for the memory of the string.
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setFileName(gridWriterHDF5_t w, const char *fileName);


/**
 * @brief  This will set whether an existing file should be overwritten.
 *
 * @param[in]  w
 *                The writer for which to work with.
 * @param[in]  force
 *                If set to @c true, then the output file will overwrite
 *                a file with the same name (should it exist).  If set
 *                to @c false, the code will fail if a file with the
 *                output name already exists.  The actual file creation
 *                happens by calling gridWriterHDF5_actiavte().
 *
 * *@return  Returns nothing.
 */
extern void
gridWriterHDF5_setForce(gridWriterHDF5_t w, bool force);


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
