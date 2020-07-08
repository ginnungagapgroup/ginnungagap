// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef checkZero_ADT_H
#define checkZero_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file checkZero/checkZero_adt.h
 * @ingroup  toolscheckZero
 * @brief  Provides the main structure of the checkZero
 *         application.
 */


/*--- Includes ----------------------------------------------------------*/
#include "checkZeroConfig.h"
#include "checkZeroSetup.h"
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"
#include "../../src/libgrid/gridReader.h"
#include "../../src/libgrid/gridWriter.h"


/*--- Implemention of main structure ------------------------------------*/

/** @brief  The main structure of the checkZero application. */
struct checkZero_struct {
	/** @brief  The setup of the application. */
	checkZeroSetup_t setup;
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
	
	gridRegularDistrib_t        distribIn;
	gridRegularDistrib_t        distribIn2;
	gridRegularDistrib_t        distribOut;
};


#endif
