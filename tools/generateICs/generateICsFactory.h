// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICSFACTORY_H
#define GENERATEICSFACTORY_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsFactory.h
 * @ingroup  toolsGICSFactory
 * @brief  Provides the interface to the factory methods to create
 *         @c generateICs applications.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICs.h"
#include "../../src/libutil/parse_ini.h"


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Constructs the application from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file from which to construct the application.
 * @param[in]      *sectionName
 *                    The name of section in which to look for the setup
 *                    information.  This may be @c NULL in which case the
 *                    value of #GENERATEICSCONFIG_DEFAULT_SECTIONNAME is
 *                    used.
 *
 * @return  Returns a new application.
 */
extern generateICs_t
generateICsFactory_newFromIni(parse_ini_t ini, const char *sectionName);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSFactory Factory Methods
 * @ingroup  toolsGICS
 * @brief  Provides ways to generate a @ref toolsGICSApp.
 *
 * Write documentation here.
 */


#endif
