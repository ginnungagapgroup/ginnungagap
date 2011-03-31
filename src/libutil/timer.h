// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef TIMER_H
#define TIMER_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/timer.h
 * @ingroup libutilMisc
 * @brief  This file provides the interface of the timer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern double
timer_start(const char *text);

extern double
timer_stop(double timing);


#endif
