// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LARE_H
#define LARE_H

/*--- Doxygen file description ------------------------------------------*/

/**
 * @file src/liblare/lare.h
 * @ingroup  liblare
 * @brief  Provides the inteface to lare library.
 */

/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include <stdint.h>
#include "../libgrid/gridPoint.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct lare_struct *lare_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern lare_t
lare_new(gridPointUint32_t dims, uint32_t numElements);

extern void
lare_del(lare_t *lare);

extern uint32_t
lare_getNumElements(const lare_t lare);

extern void
lare_getDims(const lare_t lare, gridPointUint32_t dims);

extern void
lare_getElement(const lare_t      lare,
                gridPointUint32_t element,
                uint32_t          idxOfElement);

extern void
lare_setElement(lare_t                  lare,
                const gridPointUint32_t element,
                uint32_t                idxOfElement);

extern void
lare_addElement(lare_t                  lare,
                const gridPointUint32_t element);

/**
 * @brief  implements 1d periodical clustering analysis to find the 
 * maximal region which is not covered by the mask. The output
 * is the center of the inverted empty region.
 *
 * @param[in,out]   lare
 * 						the lare structure
 * 					center
 * 						3-element array of center coordinates
 * 						in dimensionless units (Boxsize = 1.0)
 *                     
 *
 * @return  Returns nothing.
 */                
extern void
lare_getCenter(lare_t lare, float *center);


#endif

/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup liblare liblare
 * @brief  Provides the Lagrangian Region library.
 *
 * Write documentation here.
 */
