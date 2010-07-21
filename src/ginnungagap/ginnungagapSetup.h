// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPSETUP_H
#define GINNUNGAGAPSETUP_H


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include <stdint.h>
#include <stdbool.h>
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct ginnungagapSetup_struct *ginnungagapSetup_t;


/*--- Implementation of main structure ----------------------------------*/
struct ginnungagapSetup_struct {
	uint32_t dim1D;
	double   boxsizeInMpch;
	double   zInit;
	char     *gridName;
	bool     useConstraints;
	char     *fileNameConstraints;
	uint32_t dim1DConstraints;
	bool     forceSigma8InBox;
#ifdef WITH_MPI
	int      nProcs[NDIM];
#endif
};


/*--- Prototypes of exported functions ----------------------------------*/
extern ginnungagapSetup_t
ginnungagapSetup_new(parse_ini_t ini);

extern void
ginnungagapSetup_del(ginnungagapSetup_t *setup);


#endif
