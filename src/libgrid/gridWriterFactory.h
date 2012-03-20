// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERFACTORY_H
#define GRIDWRITERFACTORY_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterFactory.h
 * @ingroup  libgridIOOutFactory
 * @brief  Provides functions to contruct grid writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include "../libutil/parse_ini.h"


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Factory function to generate a generic writer from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file from which to read.  Passing @c NULL is
 *                    undefined.
 * @param[in]      *sectionName
 *                    The name of the section in which to look for the
 *                    writer information.
 *
 * @return  Returns a new grid writer.
 */
extern gridWriter_t
gridWriterFactory_newWriterFromIni(parse_ini_t ini,
                                   const char *sectionName);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libgridIOOutFactory Factory Methods for Grid Writer
 * @ingroup libgridIOOut
 * @brief This provides functionality to generate grid writers.
 *
 * @section libgridIOOutIniFormat Ini Format for Grid Writer
 *
 * @code
 * [SectionName]
 * 
 * #################
 * # Required keys #
 * #################
 * type = <string>
 * prefix = <string>
 * 
 * #################
 * # Optional keys #
 * #################
 * writerSection = <string>
 * overwriteFileIfExists = <true|false>
 * path = <string>
 * qualifier = <string>
 * suffix = <string>
 * @endcode
 *
 * The @c type key may signify to use any of the following writers:
 * <dl>
 *  <dt>Grafic</dt>
 *  <dd>The name is given by #local_typeGraficStr.  For further
 *      constrution details see @ref libgridIOOutGraficIniFormat.</dd>
 *  <dt>Silo</dt>
 *  <dd>The name is given by #local_typeSiloStr.  For further
 *      construction details see @ref libgridIOOutSiloIniFormat.</dd>
 *  <dt>HDF5</dt>
 *  <dd>The name is given by #local_typeHDF5Str.  For further
 *      construction details see @ref libgridIOOutHDF5IniFormat.</dd>
 * </dl>
 * Additional construction information for the actual reader is taken from
 * the current section, or -- if present -- from the section specified by
 * the optional key @c writerSection.
 *
 * The key @c overwriteFileIfExists can be used to tell the writer to
 * overwrite existing files, should they exist.  The default for this
 * behaviour is to never overwrite existing files.
 * 
 * The specifics of the file name can be adjusted by using the fields
 * @c path, @c qualifier, and @c suffix.  Note that @c suffix is generally
 * set to a default value of the respective writer type, and @c qualifier is
 * used elsewhere in the code, potentially overwritten the choices made
 * here.
 */


#endif
