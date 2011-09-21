// Copyright (C) 2010, 2011, Steffen Knollmann
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
typedef struct makeMask_struct *makeMask_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern makeMask_t
makeMask_newFromIni(parse_ini_t ini, const char *maskSectionName);

extern void
makeMask_run(makeMask_t msr);

extern void
makeMask_del(makeMask_t *msr);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsMakeMask makeMask
 * @ingroup  tools
 * @brief  Provides the makeMask tool.
 */


#endif
