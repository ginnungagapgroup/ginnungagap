// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef REALSPACECONSTRAINTS_ADT_H
#define REALSPACECONSTRAINTS_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file realSpaceConstraints/realSpaceConstraints_adt.h
 * @ingroup  toolsRealSpaceConstraints
 * @brief  Provides the main structure of the realSpaceConstraints
 *         application.
 */


/*--- Includes ----------------------------------------------------------*/
#include "realSpaceConstraintsConfig.h"
#include "realSpaceConstraintsSetup.h"
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"
#include "../../src/libgrid/gridReader.h"
#include "../../src/libgrid/gridWriter.h"


/*--- Implemention of main structure ------------------------------------*/

/** @brief  The main structure of the realSpaceConstraints application. */
struct realSpaceConstraints_struct {
	/** @brief  The setup of the application. */
	realSpaceConstraintsSetup_t setup;
	/** @brief  The input grid. */
	gridRegular_t               gridIn;
	/** @brief  The output grid. */
	gridRegular_t               gridOut;
	/** @brief  The reader for the input grid. */
	gridReader_t                reader;
	/** @brief  The writer for the output grid. */
	gridWriter_t                writer;
	/** @brief  The writer for the input grid. */
	gridWriter_t                writerIn;
};


#endif
