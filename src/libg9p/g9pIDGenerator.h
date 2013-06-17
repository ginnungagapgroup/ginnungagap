// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PIDGENERATOR_H
#define G9PIDGENERATOR_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pIDGenerator.h
 * @ingroup  libg9pIDGenerator
 * @brief  The interface to the ID generator.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchy.h"
#include "../libgrid/gridPoint.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct g9pIDGenerator_struct *g9pIDGenerator_t;


/*--- The id type -------------------------------------------------------*/
typedef uint64_t g9pID_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern g9pIDGenerator_t
g9pIDGenerator_new(g9pHierarchy_t h, uint8_t idLevel);

extern g9pIDGenerator_t
g9pIDGenerator_getRef(g9pIDGenerator_t idGen);

extern void
g9pIDGenerator_del(g9pIDGenerator_t *idGen);

extern g9pID_t
g9pIDGenerator_getMaxID(const g9pIDGenerator_t idGen);

extern g9pID_t
g9pIDGenerator_calcID(const g9pIDGenerator_t  idGen,
                      const gridPointUint32_t coord,
                      const uint8_t           coordLevel);

extern void
g9pIDGenerator_calcCoord(const g9pIDGenerator_t idGen,
                         g9pID_t                id,
                         gridPointUint32_t      coord,
                         const uint8_t          coordLevel);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pIDGenerator ID Generator
 * @ingroup libg9p
 * @brief Provides a generator for ID.
 */

#endif
