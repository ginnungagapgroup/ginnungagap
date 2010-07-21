// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPWN_H
#define GINNUNGAGAPWN_H


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"
#include "../libutil/parse_ini.h"
#include "../libgrid/gridRegular.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct ginnungagapWN_struct *ginnungagapWN_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern ginnungagapWN_t
ginnungagapWN_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
ginnungagapWN_del(ginnungagapWN_t *wn);

extern void
ginnungagapWN_setup(ginnungagapWN_t wn,
                    gridRegular_t   grid,
                    int             idxOfDensVar);

extern void
ginnungagapWN_dump(ginnungagapWN_t wn, gridRegular_t grid);

extern void
ginnungagapWN_reset(ginnungagapWN_t wn);

#endif
