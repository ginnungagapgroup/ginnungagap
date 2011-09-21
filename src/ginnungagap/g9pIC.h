// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PIC_H
#define G9PIC_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pIC.h
 * @ingroup  ginnungagapIC
 * @brief  Provides the interface to IC routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdint.h>
#include "../libgrid/gridRegularFFT.h"
#include "../libcosmo/cosmoPk.h"
#include "../libcosmo/cosmoModel.h"


/*--- Typedefs ----------------------------------------------------------*/
typedef enum {
	G9PIC_MODE_VX,
	G9PIC_MODE_VY,
	G9PIC_MODE_VZ
} g9pICMode_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern void
g9pIC_calcDeltaFromWN(gridRegularFFT_t gridFFT,
                      uint32_t         dim1D,
                      double           boxsizeInMpch,
                      cosmoPk_t        pk);

extern void
g9pIC_calcVelFromDelta(gridRegularFFT_t gridFFT,
                       uint32_t         dim1D,
                       double           boxsizeInMpch,
                       cosmoModel_t     model,
                       double           aInit,
                       g9pICMode_t      mode);

extern cosmoPk_t
g9pIC_calcPkFromDelta(gridRegularFFT_t gridFFT,
                      uint32_t         dim1D,
                      double           boxsizeInMpch);

extern const char *
g9pIC_getModeStr(g9pICMode_t mode);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapIC  Calculation of Initial Conditions
 * @ingroup ginnungagap
 * @brief Provides the actual IC calculations.
 */


#endif
