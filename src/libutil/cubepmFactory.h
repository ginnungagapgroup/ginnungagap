// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef CUBEPMFACTORY_H
#define CUBEPMFACTORY_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  cubepmFactory.h
 * @ingroup  libutilFilesCubePM
 * @brief  Provides the interface to the factory for generating CubePM
 *         readers.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "cubepm.h"
#include "parse_ini.h"


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Constructs a CubePM object from an ini file.
 *
 * @param[in,out]  ini
 *                   The ini file from which to read.  Passing @c NULL
 *                   is undefined.
 * @param[in]      sectionName
 *                   The name of section in which to look for the
 *                   construction information for the CubePM object.
 *                   Passing @c NULL is undefined.
 *
 * @return  Returns a new CubePM object according to the information in
 *          the file.
 */
extern cubepm_t
cubepmFactory_fromIni(parse_ini_t ini, const char *sectionName);


#endif
