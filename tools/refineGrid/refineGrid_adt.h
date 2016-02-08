// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef refineGrid_ADT_H
#define refineGrid_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file refineGrid/refineGrid_adt.h
 * @ingroup  toolsrefineGrid
 * @brief  Provides the main structure of the refineGrid
 *         application.
 */


/*--- Includes ----------------------------------------------------------*/
#include "refineGridConfig.h"
#include "refineGridSetup.h"
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"
#include "../../src/libgrid/gridReader.h"
#include "../../src/libgrid/gridWriter.h"


/*--- Implemention of main structure ------------------------------------*/

/** @brief  The main structure of the refineGrid application. */
struct refineGrid_struct {
	/** @brief  The setup of the application. */
	refineGridSetup_t setup;
	/** @brief  The input grid. */
	gridRegular_t               gridIn;
	/** @brief  The input grid to add. */
	gridRegular_t               gridIn2;
	/** @brief  The output grid. */
	gridRegular_t               gridOut;
	/** @brief  The reader for the input grid. */
	gridReader_t                reader;
	/** @brief  The reader for the second input grid. */
	gridReader_t                reader2;
	/** @brief  The writer for the output grid. */
	gridWriter_t                writer;
};


#endif
