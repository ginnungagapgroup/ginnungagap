// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASKSHAPELET_H
#define G9PMASKSHAPELET_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMaskShapelet.h
 * @ingroup  libg9pMaskShapelet
 * @brief  Provides the interface to the mask shapelet.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdint.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct g9pMaskShapelet_struct *g9pMaskShapelet_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern g9pMaskShapelet_t
g9pMaskShapelet_new(int8_t lowLevel, int8_t highLevel);

extern void
g9pMaskShapelet_del(g9pMaskShapelet_t *shapelet);

extern int8_t
g9pMaskShapelet_getLowLevel(const g9pMaskShapelet_t sl);

extern int8_t
g9pMaskShapelet_getHighLevel(const g9pMaskShapelet_t sl);

extern int8_t
g9pMaskShapelet_getExtent(const g9pMaskShapelet_t sl);

extern int8_t
g9pMaskShapelet_getDim1D(const g9pMaskShapelet_t sl);

extern const int8_t *
g9pMaskShapelet_getData(const g9pMaskShapelet_t sl);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pMaskShapelet Shapelet
 * @ingroup libg9pMask
 * @brief Provides the shapelet.
 */

#endif
