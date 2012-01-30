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
 */


#endif
