// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICSIN_H
#define GENERATEICSIN_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsIn.h
 * @ingroup  toolsGICSIn
 * @brief  Provides a wrapper for input things.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "../../src/libgrid/gridReader.h"
#include "../../src/libdata/dataVar.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct generateICsIn_struct *generateICsIn_t;


/*--- Structure definition ----------------------------------------------*/
struct generateICsIn_struct {
	gridReader_t velx;
	gridReader_t vely;
	gridReader_t velz;
	// Auto generated
	dataVar_t    varVelx;
	dataVar_t    varVely;
	dataVar_t    varVelz;
};


/*--- Prototypes of exported functions ----------------------------------*/
extern generateICsIn_t
generateICsIn_new(gridReader_t velx, gridReader_t vely, gridReader_t velz);

extern void
generateICsIn_del(generateICsIn_t *generateICsIn);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSIn Input Wrapper
 * @ingroup toolsGICS
 * @brief Simple module to wrap the input reader.
 */

#endif
