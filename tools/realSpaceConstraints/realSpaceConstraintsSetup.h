// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef REALSPACECONSTRAINTSSETUP_H
#define REALSPACECONSTRAINTSSETUP_H


/*--- Includes ----------------------------------------------------------*/
#include "realSpaceConstraintsConfig.h"
#include <stdint.h>
#include <stdbool.h>
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct realSpaceConstraintsSetup_struct *
realSpaceConstraintsSetup_t;


/*--- Implemention of main structure ------------------------------------*/
struct realSpaceConstraintsSetup_struct {
	double   boxsizeInMpch;
	uint32_t inputDim1D;
	uint32_t outputDim1D;
	bool     useFileForInput;
	char     *readerSecName;
	char     *writerSecName;
	char     *writerInSecName;
	int      seedIn;
	int      seedOut;
};


/*--- Prototypes of exported functions ----------------------------------*/
extern realSpaceConstraintsSetup_t
realSpaceConstraintsSetup_newFromIni(parse_ini_t ini,
                                     const char  *sectionName);

extern void
realSpaceConstraintsSetup_del(realSpaceConstraintsSetup_t *setup);

#endif
