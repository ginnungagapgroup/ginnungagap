// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPIC_H
#define GINNUNGAGAPIC_H


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include <stdint.h>
#include "../libgrid/gridRegularFFT.h"
#include "../libcosmo/cosmoPk.h"
#include "../libcosmo/cosmoModel.h"

/*--- Typedefs ----------------------------------------------------------*/
typedef enum {
	GINNUNGAGAPIC_MODE_VX,
	GINNUNGAGAPIC_MODE_VY,
	GINNUNGAGAPIC_MODE_VZ
} ginnungagapICMode_t;

/*--- Prototypes of exported functions ----------------------------------*/
extern void
ginnungagapIC_calcDeltaFromWN(gridRegularFFT_t gridFFT,
                              uint32_t         dim1D,
                              double           boxsizeInMpch,
                              cosmoPk_t        pk);

extern void
ginnungagapIC_calcVelFromDelta(gridRegularFFT_t    gridFFT,
                               uint32_t            dim1D,
                               double              boxsizeInMpch,
                               cosmoModel_t        model,
                               double              aInit,
                               ginnungagapICMode_t mode);

extern cosmoPk_t
ginnungagapIC_calcPkFromDelta(gridRegularFFT_t gridFFT,
                              uint32_t         dim1D,
                              double           boxsizeInMpch);
extern const char *
ginnungagapIC_getModeStr(ginnungagapICMode_t mode);


#endif
