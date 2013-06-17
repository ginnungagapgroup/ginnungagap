// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASKCREATOR_H
#define G9PMASKCREATOR_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMaskCreator.h
 * @ingroup  libg9pMaskCreator
 * @brief  The interface to the mask creator.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMask.h"
#include "../libgrid/gridPoint.h"


/*--- Prototypes of exported functions ----------------------------------*/

extern void
g9pMaskCreator_fromCells(g9pMask_t               mask,
                         uint64_t                numCells,
                         const gridPointUint32_t *cells);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pMaskCreator Mask Creator
 * @ingroup libg9pMask
 * @brief Provides methods to create the actual mask data.
 */

#endif
