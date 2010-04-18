// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef TIMER_H
#define TIMER_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern double
timer_start(const char *text);

extern double
timer_stop(double timing);


#endif
