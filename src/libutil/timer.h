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

/**
 * @brief  This start a timer.
 *
 * @param[in]  *text
 *                A short string that should give a name of what is
 *                being timed.
 *
 * @return  Returns a number that can be passed to timer_stop() to
 *          evaluate the elapsed time in second.s
 */
extern double
timer_start(const char *text);

/**
 * @brief  This will stop a timer.
 *
 * @param[in]  timing
 *                The value obtained from a previous call of
 *                timer_start().
 *
 * @return  Returns the number of seconds elapsed between the call of
 *          timer_start() and timer_stop().
 */
extern double
timer_stop(double timing);


#endif
