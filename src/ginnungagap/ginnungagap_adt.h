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


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagap_struct {
	g9pSetup_t           setup;
	cosmoModel_t         model;
	cosmoPk_t            pk;
	g9pWN_t              whiteNoise;
	gridRegular_t        grid;
	gridRegularDistrib_t gridDistrib;
	gridRegularFFT_t     gridFFT;
	gridWriter_t         finalWriter;
	int                  posOfDens;
	int                  rank;
	int                  size;
	int                  numThreads;
};


#endif
