// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMO_H
#define COSMO_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmoModel.h"
#include "cosmoPk.h"


/*--- Exported defines --------------------------------------------------*/
#define cosmo_z2a(z) (1./((z)+1.))
#define cosmo_a2z(a) (1./(a) - 1.)


#endif
