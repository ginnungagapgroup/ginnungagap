// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PMASKCREATOR_TESTS_H
#define G9PMASKCREATOR_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pMaskCreator_tests.h
 * @ingroup  libg9pMaskCreatorTest
 * @brief  Interface to the test routines for @ref libg9pMaskCreator.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
g9pMaskCreator_verifyEmptyMaskHasMinLevelThroughout(void);

extern bool
g9pMaskCreator_verifyMaskIfOneCellIsTagged(void);

extern bool
g9pMaskCreator_verifyMaskIfOneCellsAffectsMultipleTiles(void);

extern bool
g9pMaskCreator_verifyMaskIfOneCellsAffectsMultTilesPeriodic(void);

extern bool
g9pMaskCreator_verifyMaskIfThreeCellsAreTaggedInOneTile(void);

extern bool
g9pMaskCreator_verifyMaskIfTwoCellsAreTaggedSlightOverlap(void);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pMaskCreatorTest Mask Creator Tests
 * @ingroup libg9pMaskCreator
 * @brief Provides tests for the creation routines of
 *        @ref libg9pMaskCreator.
 */


#endif
