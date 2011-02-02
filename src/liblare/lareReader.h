// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LAREREADER_H
#define LAREREADER_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lare.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct lareReader_struct *lareReader_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern lareReader_t
lareReader_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
lareReader_del(lareReader_t *reader);

extern lare_t
lareReader_read(lareReader_t reader);


#endif
