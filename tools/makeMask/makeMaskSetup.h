// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKEMASKSETUP_H
#define MAKEMASKSETUP_H


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct makeMaskSetup_struct *makeMaskSetup_t;


/*--- Implemention of main structure ------------------------------------*/
struct makeMaskSetup_struct {
	uint32_t baseGridSize1D;
	uint32_t numLevels;
	uint32_t refinementFactor;
	uint32_t baseRefinementLevel;
	char     *outSecName;
#ifdef WITH_MPI
	int      nProcs[NDIM];
#endif
};


/*--- Prototypes of exported functions ----------------------------------*/
extern makeMaskSetup_t
makeMaskSetup_newFromIni(parse_ini_t ini, const char *maskSectionName);

extern void
makeMaskSetup_del(makeMaskSetup_t *setup);

extern uint32_t
makeMaskSetup_getBaseGridSize1D(const makeMaskSetup_t setup);

extern char *
makeMaskSetup_getOutSecName(const makeMaskSetup_t setup);

#endif
