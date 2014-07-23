// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GENERATEICS_ADT_H
#define GENERATEICS_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICs_adt.h
 * @ingroup  toolsGICSApp
 * @brief  Provides the main structure of the generateICs
 *         application.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"

#include "generateICsData.h"
#include "generateICsMode.h"
#include "generateICsIn.h"
#include "generateICsOut.h"

#include "../../src/libgrid/gridReader.h"
#include "../../src/libcosmo/cosmoModel.h"
#include "../../src/libg9p/g9pHierarchy.h"
#include "../../src/libg9p/g9pDataStore.h"
#include "../../src/libg9p/g9pMask.h"


/*--- Implemention of main structure ------------------------------------*/

/** @brief  The main structure of the generateICs application. */
struct generateICs_struct {
	/** @brief  Stores the size of MPI communicator. */
	int size;
	/** @brief  Stores the rank of this MPI task. */
	int rank;
	/** @brief  Stores the number of threads that should be used. */
	int numThreads;

	/** @brief  Stores the operational mode. */
	generateICsMode_t mode;
	/** @brief  Stores the resolution independent data. */
	generateICsData_t data;

	/** @brief  Stores the hierarchy. */
	g9pHierarchy_t hierarchy;
	/** @brief  Stores the data store object (maps to file names). */
	g9pDataStore_t datastore;
	/** @brief  Stores the mask. */
	g9pMask_t      mask;

	/** @brief  Stores the input information. */
	generateICsIn_t in;
	/** @brief  Stores the output information. */
	generateICsOut_t out;
	
	int32_t zoomlevel;
	int32_t *typeForLevel;
};


#endif
