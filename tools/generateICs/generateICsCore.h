// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICSCORE_H
#define GENERATEICSCORE_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsCore.h
 * @ingroup  toolsGICSCore
 * @brief  Interface to core functions for generateICs.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICsData.h"
#include "generateICsMode.h"
#include "../../src/libgrid/gridPatch.h"


/*--- Simple structure easing the data passing --------------------------*/
struct generateICsCore_struct {
	gridPatch_t             patch;
	gridPointUint32_t       fullDims;
	uint64_t                numParticles;
	fpv_t                   *pos;
	fpv_t                   *vel;
	void                    *id;
	const generateICsData_t data;
	const generateICsMode_t mode;
};

typedef struct generateICsCore_struct        generateICsCore_s;
typedef struct generateICsCore_struct        *generateICsCore_t;
typedef struct generateICsCore_struct *const generateICsCore_const_t;


/*--- Exported defines --------------------------------------------------*/
#define GENICSCORE_INIT_STRUCT(d, m) \
	{                                \
		.patch = NULL,               \
		.pos   = NULL,               \
		.vel   = NULL,               \
		.id    = NULL,               \
		.data  = (d),                \
		.mode  = (m),                \
	}


/*--- Prototypes of exported functions ----------------------------------*/
extern void
generateICsCore_toParticles(generateICsCore_const_t d);

extern void
generateICsCore_initPosID(generateICsCore_const_t d);

extern void
generateICsCore_vel2pos(generateICsCore_const_t d);

extern void
generateICsCore_convertVel(generateICsCore_const_t d);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSCore Core Functionality
 * @ingroup toolsGICS
 * @brief  Creating particles from velocity fields.
 *
 * Provides a set of functions that will do the heavy lifting of creating
 * particles from set of (masked) velocity field.
 */


#endif
