// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PWN_H
#define G9PWN_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pWN.h
 * @ingroup  ginnungagapWN
 * @brief  Provides the interface to the WN routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "../libutil/parse_ini.h"
#include "../libgrid/gridRegular.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct g9pWN_struct *g9pWN_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern g9pWN_t
g9pWN_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
g9pWN_del(g9pWN_t *wn);

extern void
g9pWN_setup(g9pWN_t       wn,
            gridRegular_t grid,
            int           idxOfDensVar);

extern void
g9pWN_dump(g9pWN_t wn, gridRegular_t grid);

extern void
g9pWN_reset(g9pWN_t wn);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapWN  White Noise
 * @ingroup ginnungagap
 * @brief Provides the WN functionality.
 */


#endif
