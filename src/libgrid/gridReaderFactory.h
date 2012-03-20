// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERFACTORY_H
#define GRIDREADERFACTORY_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderFactory.h
 * @ingroup  libgridIOInFactory
 * @brief  Provides functions to contruct grid reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "../libutil/parse_ini.h"
#include "../libutil/filename.h"
#include <stdlib.h>


/*--- Exported defines --------------------------------------------------*/

/** @brief  Constant to signal that no file-name is passed and it should be
 *          read from the specific section of the ini file. */
#define GRIDREADERFACTORY_GET_FILENAME_FROM_SPECIFIC_SECTION NULL


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creates a new reader object from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file in which to look for the construction
 *                    information for this reader.
 * @param[in]      *sectionName
 *                    The section name within the ini file that holds
 *                    the construction information.
 *
 * @return  Returns a new reader object that is constructed from the
 *          information in the ini file.
 */
extern gridReader_t
gridReaderFactory_newReaderFromIni(parse_ini_t ini, const char *sectionName);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOInFactory Factory Methods for Grid Reader
 * @ingroup libgridIOIn
 * @brief This provides functionality to generate grid reader.
 *
 * @section libgridIOInIniFormat Ini Format for Grid Reader
 *
 * @code
 * [SectionName]
 * # Required, the type of the reader
 * readerType = <string>
 * # Optional, the section from which to read type specific information
 * readerSection = <string>
 * # Optional, the path part of the file name.
 * path = <string>
 * # Required, the prefix part of the file name.
 * prefix = <string>
 * # Optional, the qualifier part of the file name.
 * qualifier = <string>
 * # Optional, the suffix part of the file name.
 * suffix = <string>
 * @endcode
 *
 * <dl>
 *  <dt>Grafic</dt>
 *  <dd>The name is given by @ref local_typeGraficStr.  For further
 *      constrution details see @ref libgridIOInGraficIniFormat.</dd>
 *  <dt>BOV</dt>
 *  <dd>The name is given by #local_typeBovStr.  For further
 *      construction details see @ref libgridIOInBovIniFormat.</dd>
 *  <dt>Silo</dt>
 *  <dd>The name is given by #local_typeSiloStr.  For further
 *      construction details see @ref libgridIOInSiloIniFormat.</dd>
 *  <dt>HDF5</dt>
 *  <dd>The name is given by #local_typeHDF5Str.  For further
 *      construction details see @ref libgridIOInHDF5IniFormat.</dd>
 * </dl>
 *
 * @section libgridIOInGraficIniFormat  Grafic
 *
 * @code
 * [SectionName]
 * @endcode
 *
 * @section libgridIOInBovIniFormat  BOV
 *
 * @code
 * [SectionName]
 * @endcode
 *
 * @section libgridIOInSiloIniFormat  Silo
 *
 * @code
 * [SectionName]
 * @endcode
 *
 * @section libgridIOInHDF5IniFormat  HDF5
 *
 * @code
 * [SectionName]
 * @endcode
 */


#endif
