// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADER_H
#define GRIDREADER_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridReader_struct *gridReader_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridReader_t
gridReader_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
gridReader_del(gridReader_t *gridReader);


#endif
