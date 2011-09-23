// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERHDF5_H
#define GRIDREADERHDF5_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5.h
 * @ingroup  libgridIOInHDF5
 * @brief  Provides the interface for the HDF5 reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Handle for the HDF5 reader. */
typedef struct gridReaderHDF5_struct *gridReaderHDF5_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @name Creating and Deleting
 *
 * @{
 */
extern gridReaderHDF5_t
gridReaderHDF5_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridReaderHDF5_del(gridReader_t *reader);


/** @} */

/**
 * @name  Using
 */

extern void
gridReaderHDF5_readIntoPatch(gridReader_t reader, gridPatch_t patch);

extern void
gridReaderHDF5_readIntoPatchForVar(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar);


/** @} */


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInHDF5 HDF5 Reader
 * @ingroup libgridIOIn
 * @brief  Provides the HDF5 reader.
 *
 * @section libgridIOInHDF5IniFormat  Expected Format for Ini Files
 *
 * @code
 * @endcode
 */

#endif
