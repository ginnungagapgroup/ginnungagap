// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PINIT_H
#define G9PINIT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pInit.h
 * @ingroup  ginnungagapInit
 * @brief  Provides the interface to the initialisation routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pNorm.h"
#include <stdbool.h>
#include "../libcosmo/cosmoPk.h"
#include "../libcosmo/cosmoModel.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern void
g9pInit_init(double         boxsizeInMpch,
             uint32_t       dim1D,
             double         zInit,
             cosmoPk_t      pk,
             cosmoModel_t   model,
             g9pNorm_mode_t normalisationMode);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapInit  Initialisation
 * @ingroup ginnungagap
 * @brief Provides initialisation of ginnungagap.
 */


#endif
