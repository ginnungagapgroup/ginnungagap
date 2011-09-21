// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAP_H
#define GINNUNGAGAP_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file ginnungagap.h
 * @ingroup  ginnungagap
 * @brief  Provides the interface to the ginnungagap code.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct ginnungagap_struct *ginnungagap_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini);

extern void
ginnungagap_init(ginnungagap_t g9p);

extern void
ginnungagap_run(ginnungagap_t g9p);

extern void
ginnungagap_del(ginnungagap_t *g9p);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagap Ginnungagap
 * @brief Provides the ginnungagap code.
 */


#endif
