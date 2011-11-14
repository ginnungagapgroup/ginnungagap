// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAP_ADT_H
#define GINNUNGAGAP_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file ginnungagap_adt.h
 * @ingroup  ginnungagap
 * @brief  Provides implementation of the main structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pSetup.h"
#include "g9pWN.h"
#include "../libcosmo/cosmoModel.h"
#include "../libcosmo/cosmoPk.h"
#include "../libgrid/gridRegular.h"
#include "../libgrid/gridRegularDistrib.h"
#include "../libgrid/gridRegularFFT.h"
#include "../libgrid/gridWriter.h"
#include "../libgrid/gridHistogram.h"


/*--- Implemention of main structure ------------------------------------*/

/** @brief Main structure */
struct ginnungagap_struct {
	/** @brief  The code setup. */
	g9pSetup_t           setup;
	/** @brief  The cosmological model used. */
	cosmoModel_t         model;
	/** @brief  The power spectrum used. */
	cosmoPk_t            pk;
	/** @brief  The white noise module. */
	g9pWN_t              whiteNoise;
	/** @brief  The main grid. */
	gridRegular_t        grid;
	/** @brief  The distribution of the main grid (parallel interface). */
	gridRegularDistrib_t gridDistrib;
	/** @brief  The FFT module for the grid. */
	gridRegularFFT_t     gridFFT;
	/** @brief  The writer used to write the velocity fields. */
	gridWriter_t         finalWriter;
	/** @brief  The position of the density variable in the grid. */
	int                  posOfDens;
	/** @brief  The MPI rank of this tasks. */
	int                  rank; ///< Will be 0 for non-MPI situations.
	/** @brief  The total number of MPI tasks. */
	int                  size; ///< Will be 1 for non-MPI situations.
	/** @brief  The number of OpenMP threads used. */
	int                  numThreads; ///< Will be 1 for non-OpenMP situations
	/** @brief  The histogram used for densities. */
	gridHistogram_t      histoDens;
	/** @brief  The histogram used for velocities. */
	gridHistogram_t      histoVel;
};


#endif
