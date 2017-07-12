// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICSMODE_H
#define GENERATEICSMODE_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsMode.h
 * @ingroup  toolsGICSMode
 * @brief  Provide a wrapper structure for the operating mode.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct generateICsMode_struct *generateICsMode_t;


/*--- Structure definition ----------------------------------------------*/
struct generateICsMode_struct {
	const bool doGas;
	const bool useLongIDs;
	const bool autoCenter;
	const bool kpc;
	const bool sequentialIDs;
	const bool doMassBlock;
};


/*--- Prototypes of exported functions ----------------------------------*/
extern generateICsMode_t
generateICsMode_new(const bool doGas, const bool useLongIDs, const bool autoCenter, 
                    const bool kpc, const bool sequentialIDs, const bool doMassBlock);

extern void
generateICsMode_del(generateICsMode_t *mode);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSMode Mode
 * @ingroup toolsGICS
 * @brief Provides a simple wrapper for the operational mode.
 */

#endif
