// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICSDATA_H
#define GENERATEICSDATA_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsData.h
 * @ingroup  toolsGICSData
 * @brief  Provides structure and interface for common data.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "../../src/libcosmo/cosmoModel.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct generateICsData_struct *generateICsData_t;


/*--- Structure definition ----------------------------------------------*/
struct generateICsData_struct {
	// Input
	const double       boxsizeInMpch;
	const double       aInit;
	const cosmoModel_t model;
	// Computed from model
	const double       vFact;
	const double       adot;
	const double       growthVel;
	// Free to set
	double             posFactor;
	double             velFactor;
};


/*--- Prototypes of exported functions ----------------------------------*/
extern generateICsData_t
generateICsData_new(double       boxsizeInMpch,
                    double       aInit,
                    cosmoModel_t model);

extern void
generateICsData_del(generateICsData_t *data);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGICSData Data
 * @ingroup toolsGICS
 * @brief Provides a simple interface to common data.
 */

#endif
