// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PIDGENERATOR_ADT_H
#define G9PIDGENERATOR_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pIDGenerator_adt.h
 * @ingroup  libg9pIDGenerator
 * @brief  Provides the main structure for the ID generator.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "../libutil/refCounter.h"
#include "g9pHierarchy.h"


/*--- ADT implementation ------------------------------------------------*/
struct g9pIDGenerator_struct {
	refCounter_t      refCounter;
	g9pHierarchy_t    h;
	uint8_t           idLevel;
	gridPointUint32_t idDims;
	g9pID_t           maxID;
};


#endif
