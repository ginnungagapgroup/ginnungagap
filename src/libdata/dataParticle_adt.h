// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAPARTICLE_ADT_H
#define DATAPARTICLE_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataParticle.h"
#include <stdbool.h>
#include "../libutil/refCounter.h"
#include "../libutil/varArr.h"


/*--- ADT implementation ------------------------------------------------*/
struct dataParticle_struct {
	refCounter_t refCounter;
	char         *name;
	int          dataParticleID;
	varArr_t     vars;
	bool         isLocked;
};


#endif
