// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef REALSPACECONSTRAINTS_H
#define REALSPACECONSTRAINTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file realSpaceConstraints/realSpaceConstraints.h
 * @ingroup  toolsRealSpaceConstraints
 * @brief  Provides the interface to the realSpaceConstraints tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "realSpaceConstraintsConfig.h"
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Provides a handle for the realSpaceConstraints application. */
typedef struct realSpaceConstraints_struct *realSpaceConstraints_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creates a new realSpaceConstraints application from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file that should be used to set up the
 *                    application.
 *
 * @return  Returns a new realSpaceConstraints application.
 */
extern realSpaceConstraints_t
realSpaceConstraints_newFromIni(parse_ini_t ini);


/**
 * @brief  Executes the application.
 *
 * @param[in,out]  rsc
 *                    The realSpaceConstraints application to execute.
 *
 * @return  Returns nothing.
 */
extern void
realSpaceConstraints_run(realSpaceConstraints_t rsc);


/**
 * @brief  Deletes a realSpaceConstraints application and frees the
 *         associated memory.
 *
 * @param[in,out]  *rsc
 *                     The realSpaceApplication that should be deleted.
 *
 * @return  Returns nothing.
 */
extern void
realSpaceConstraints_del(realSpaceConstraints_t *rsc);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsRealSpaceConstraints realSpaceConstraints
 * @ingroup  tools
 * @brief  Provides the realSpaceConstraints tool.
 */


#endif
