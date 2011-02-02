// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LAREREADERLEGACY_H
#define LAREREADERLEGACY_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReader.h"
#include "lare.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct lareReaderLegacy_struct *lareReaderLegacy_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern lareReaderLegacy_t
lareReaderLegacy_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
lareReaderLegacy_del(lareReader_t *reader);

extern lare_t
lareReaderLegacy_read(lareReader_t reader);


#endif
