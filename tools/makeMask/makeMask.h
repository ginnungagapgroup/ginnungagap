// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef MAKEMASK_H
#define MAKEMASK_H


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "../../src/libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct makeMask_struct *makeMask_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern makeMask_t
makeMask_newFromIni(parse_ini_t ini, const char *maskSectionName);

extern void
makeMask_run(makeMask_t msr);

extern void
makeMask_del(makeMask_t *msr);


#endif
