// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKEMASK_H
#define MAKEMASK_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file makeMask/makeMask.h
 * @ingroup  toolsMakeMask
 * @brief  Provides the interface to the makeMask tool.
 */


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Provides a handle for the makeMask application. */
typedef struct makeMask_struct *makeMask_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Creates a new makeMask application from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file from which to read the construction
 *                    information.
 * @param[in]      *maskSectionName
 *                    The name of the section from which to construct the
 *                    application.
 *
 * @return  Returns a new application object.
 */
extern makeMask_t
makeMask_newFromIni(parse_ini_t ini, const char *maskSectionName);

/**
 * @brief  Runs the application
 *
 * @param[in,out]  mama
 *                    The application to execute.
 *
 * @return  Returns nothing.
 */
extern void
makeMask_run(makeMask_t mama);

/**
 * @brief  Deletes a makeMask application and frees the associated memory.
 *
 * @param[in,out]  *mama
 *                    The makeMask application that should be deleted.
 *
 * @return  Returns nothing.
 */
extern void
makeMask_del(makeMask_t *mama);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsMakeMask makeMask
 * @ingroup  tools
 * @brief  Provides the makeMask tool.
 *
 * This is a tool that creates from a list of high resolution cells a mask
 * in the requested base resolution that for every cells gives the
 * resolution level at which this cell should be generated.
 *
 * The following gives an example of a required ini file (see also
 * doc/examples/makeMask.ini):
 *
 * @code
 * [Mask]
 * 
 * @endcode
 */


#endif
