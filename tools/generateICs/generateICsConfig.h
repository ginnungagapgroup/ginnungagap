// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICSCONFIG_H
#define GENERATEICSCONFIG_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsConfig.h
 * @ingroup  toolsGICSConfig
 * @brief  Provides code configuration for generateICs.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"


/*--- Defines -----------------------------------------------------------*/

/** @brief  Gives the default section name for the setup from ini files. */
#define GENERATEICSCONFIG_DEFAULT_SECTIONNAME "GenerateICs"

/** @brief  Gives the default for doing gas particles or not. */
#define GENERATEICSCONFIG_DEFAULT_DOGAS false

/** @brief  Gives the default for using 64bit IDs or not. */
#define GENERATEICSCONFIG_DEFAULT_DOLONGIDS false

/** @brief  Gives the default for producing sequential IDs or not. */
#define GENERATEICSCONFIG_DEFAULT_SEQUENTIALIDS false

/** @brief  Gives the default for re-centering to zoom region or not. */
#define GENERATEICSCONFIG_DEFAULT_AUTOCENTER false

/** @brief  Gives the default for using kpc or Mpc output units. */
#define GENERATEICSCONFIG_DEFAULT_KPC false

/** @brief  Gives the default prefix for the ginnungagap section name. */
#define GENERATEICSCONFIG_DEFAULT_GINNUNGAGAPSECTION "Ginnungagap"

/** @brief  The default name of the section from which to setup the cosmology.*/
#define GENERATEICSCONFIG_DEFAULT_COSMOLOGYSECTION "Cosmology"

/** @brief  The default name of the section for the input information.*/
#define GENERATEICSCONFIG_DEFAULT_INPUTSECTION "Input"

/** @brief  The default name of the section for the output information.*/
#define GENERATEICSCONFIG_DEFAULT_OUTPUTSECTION "Output"

/** @brief  Gives the default number of resolution levels. */
#define GENERATEICSCONFIG_DEDAULT_NUMBEROFRESOLUTIONLEVELS 1

/** @brief  Gives the default prefix for the level sections. */
#define GENERATEICSCONFIG_DEFAULT_LEVELSECTIONPREFIX "Level_"

/** @brief  The default name of the section for the hierarchy.*/
#define GENERATEICSCONFIG_DEFAULT_HIERARCHYSECTION "Hierarchy"

/** @brief  The default name of the section for the mask information.*/
#define GENERATEICSCONFIG_DEFAULT_MASKSECTION "Mask"

/** @brief  Gives the default tiling of the grid buffers.  */
#define GENERATEICSCONFIG_DEFAULT_NUMTILES 1


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSConfig Code Configuration
 * @ingroup  toolsGICS
 * @brief  Provides the code configuration.
 */

/**
 * @defgroup toolsGICS  Generating initial particle distributions
 * @ingroup tools
 * @brief  Provides a tool to generate initial conditions from (a set of)
 *         velocity fields.
 */

#endif
