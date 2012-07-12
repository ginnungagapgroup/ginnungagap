// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASKIO_H
#define G9PMASKIO_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libg9p/g9pMaskIO.h
 * @ingroup  libg9pMaskIO
 * @brief  The interface to the mask IO routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMask.h"
#include "g9pHierarchy.h"
#include "../libutil/parse_ini.h"
#include "../libgrid/gridReader.h"
#include "../libgrid/gridWriter.h"


/*--- Prototypes of exported functions ----------------------------------*/

extern g9pMask_t
g9pMaskIO_newFromIni(parse_ini_t    ini,
                     const char     *sectionName,
                     g9pHierarchy_t h);

extern void
g9pMaskIO_write(g9pMask_t mask, gridWriter_t writer);

extern void
g9pMaskIO_read(g9pMask_t mask, gridReader_t reader);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup  libg9pMaskIO Mask IO
 * @ingroup  libg9pMask
 * @brief  Provides IO functionality for @ref libg9pMask.
 */

#endif
