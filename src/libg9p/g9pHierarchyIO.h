// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PHIERARCHYIO_H
#define G9PHIERARCHYIO_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchyIO.h
 * @ingroup  libg9pHierarchyIO
 * @brief  Provides the interface for @ref libg9pHierarchy IO.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchy.h"
#include "../libutil/parse_ini.h"


/*--- Prototypes of exported functions ----------------------------------*/

extern g9pHierarchy_t
g9pHierarchyIO_newFromIni(parse_ini_t ini, const char *sectionName);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pHierarchyIO Hierarchy IO
 * @ingroup libg9pHierarchy
 * @brief Provides IO routines for @ref libg9pHierarchy IO.
 */


#endif
