// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPCONFIG_H
#define GINNUNGAGAPCONFIG_H


/*--- Includes ----------------------------------------------------------*/
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct ginnungagapConfig_struct *ginnungagapConfig_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern ginnungagapConfig_t
ginnungagapConfig_new(parse_ini_t ini);

extern void
ginnungagapConfig_del(ginnungagapConfig_t *config);


#endif
