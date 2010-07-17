// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADERGRAFIC_H
#define GRIDREADERGRAFIC_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridReaderGrafic_struct *gridReaderGrafic_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridReaderGrafic_t
gridReaderGrafic_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridReaderGrafic_del(gridReader_t *reader);

extern void
gridReaderGrafic_readIntoPatch(gridReader_t reader, gridPatch_t patch);

extern void
gridReaderGrafic_readIntoPatchForVar(gridReader_t reader,
                                     gridPatch_t  patch,
                                     int          idxOfVar);


#endif
