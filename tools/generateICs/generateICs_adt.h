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
#include "../../src/libgrid/gridReader.h"
#include "../../src/libcosmo/cosmoModel.h"
#include "../../src/libg9p/g9pHierarchy.h"
#include "../../src/libg9p/g9pMask.h"


/*--- Implemention of main structure ------------------------------------*/

/** @brief  The main structure of the generateICs application. */
struct generateICs_struct {
	/** @brief  Stores the size of MPI communicator. */
	int            size;
	/** @brief  Stores the rank of this MPI task. */
	int            rank;
	/** @brief  Stores the number of threads that should be used. */
	int            numThreads;
	/** @brief  Stores the cosmological model. */
	cosmoModel_t   model;
	/** @brief  Stores the hierarchy. */
	g9pHierarchy_t hierarchy;
	/** @brief  Stores the mask. */
	g9pMask_t      mask;
	/** @brief  The boxsize in Mpc/h. */
	double         boxsizeInMpch;
	/** @brief  The initial expansion factor. */
	double         aInit;
	/** @brief  Flag to indicate whether gas particles are used. */
	bool           doGas;
	/** @brief  Flag to indicate whether long IDs are used. */
	bool           doLongIDs;
	/** @brief  The number of output files. */
	uint32_t       numFiles;
	/** @brief  The prefix of the output files. */
	char           *prefix;
};


#endif
