// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOTF_H
#define COSMOTF_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdint.h>


/*--- Exported types ----------------------------------------------------*/
typedef enum {
	COSMOTF_TYPE_EISENSTEINHU1998,
	COSMOTF_TYPE_ANATOLY2000
} cosmoTF_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern void
cosmoTF_eisensteinHu1998(double   omegaMatter0,
                         double   omegaBaryon0,
                         double   hubble,
                         double   tempCMB,
                         uint32_t numPoints,
                         double   *k,
                         double   *P);


#endif
