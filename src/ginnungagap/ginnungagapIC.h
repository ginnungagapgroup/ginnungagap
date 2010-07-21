// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPIC_H
#define GINNUNGAGAPIC_H


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "ginnungagap.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern void
ginnungagapIC_calcFromWhiteNoise(ginnungagap_t ginnungagap);

extern void
ginnungagapIC_calcPowerSpectrum(ginnungagap_t ginnungagap);


#endif
