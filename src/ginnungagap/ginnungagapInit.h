// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPINIT_H
#define GINNUNGAGAPINIT_H


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "g9pNorm.h"
#include <stdbool.h>
#include "../libcosmo/cosmoPk.h"
#include "../libcosmo/cosmoModel.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern void
ginnungagapInit_init(double         boxsizeInMpch,
                     uint32_t       dim1D,
                     double         zInit,
                     cosmoPk_t      pk,
                     cosmoModel_t   model,
                     g9pNorm_mode_t normalisationMode);


#endif
