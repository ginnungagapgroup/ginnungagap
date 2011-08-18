// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LAREREADERLOI_H
#define LAREREADERLOI_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReader.h"
#include "lare.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct lareReaderLOI_struct *lareReaderLOI_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern lareReaderLOI_t
lareReaderLOI_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
lareReaderLOI_del(lareReader_t *reader);

extern lare_t
lareReaderLOI_read(lareReader_t reader);


#endif
