// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PHIERARCHY_H
#define G9PHIERARCHY_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchy.h
 * @ingroup  libg9pHierarchy
 * @brief  Provides an interface to the grid hierarchy.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdint.h>
#include "../libgrid/gridPoint.h"


/*--- ADT handle --------------------------------------------------------*/

/** @brief  Define the handle for the hierarchy. */
typedef struct g9pHierarchy_struct *g9pHierarchy_t;


/*--- Exported Defines --------------------------------------------------*/

/**
 * @brief  Gives the maximal number of levels allowed in a hierarchy.
 *
 * This really shouldn't be larger than @c 21 in the 3D case, as the
 * smallest sensible factor is @c 2 and with a base resolution level of
 * @c 1 this translates to a highest resolution levels whose number of cell
 * is just representable by a 64bit integer.
 *
 * Most likely you should use a number of level well below this upper limit.
 */
#define G9PHIERARCHY_MAX_NUM_LEVELS 21


/*--- Prototypes of exported functions ----------------------------------*/

extern g9pHierarchy_t
g9pHierarchy_newWithDims(uint8_t numLevels, const uint32_t *const dim1Ds);

extern g9pHierarchy_t
g9pHierarchy_newWithSimpleFactor(uint8_t  numLevels,
                                 uint32_t minDim1D,
                                 uint32_t factor);

extern g9pHierarchy_t
g9pHierarchy_newWithExplicitFactors(uint8_t               numLevels,
                                    uint32_t              minDim1D,
                                    const uint32_t *const factors);

extern g9pHierarchy_t
g9pHierarchy_getRef(g9pHierarchy_t h);


/**
 * @brief  Deletes a hierarchy freeing all associated memory.
 *
 * @param[in,out]  *h
 *                    A pointer to the external variable holding the
 *                    hierarchy that should be deleted.  After deletion, the
 *                    external variable will be set to @c NULL.
 *
 * @return  Returns nothing.
 */
extern void
g9pHierarchy_del(g9pHierarchy_t *h);

extern uint8_t
g9pHierarchy_getNumLevels(const g9pHierarchy_t h);

/**
 *
 */
extern const uint32_t *
g9pHierarchy_getDim1Ds(const g9pHierarchy_t h, uint32_t *dims);

extern uint32_t
g9pHierarchy_getDim1DAtLevel(const g9pHierarchy_t h, uint8_t level);

extern const uint32_t *
g9pHierarchy_getFactors(const g9pHierarchy_t h, uint32_t *factors);

extern uint32_t
g9pHierarchy_getFactorToNextLevel(const g9pHierarchy_t h, uint8_t thisLevel);

extern uint32_t
g9pHierarchy_getFactorFromPrevLevel(const g9pHierarchy_t h,
                                    uint8_t              thisLevel);

extern uint32_t
g9pHierarchy_getFactorBetweenLevel(const g9pHierarchy_t h,
                                   uint8_t              l1,
                                   uint8_t              l2);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pHierarchy Hierarchy
 * @ingroup libg9p
 * @brief Provides the hierarchy of resolution levels.
 */


#endif
