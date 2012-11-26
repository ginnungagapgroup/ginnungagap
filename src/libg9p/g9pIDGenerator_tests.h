// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PIDGENERATOR_TESTS_H
#define G9PIDGENERATOR_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pIDGenerator_tests.h
 * @ingroup  libg9pIDGenerator
 * @brief  Provides the interface to the test functions for the ID
 *         generator.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
g9pIDGenerator_verifyCreation(void);

extern bool
g9pIDGenerator_verifyIDGeneration(void);

extern bool
g9pIDGenerator_verifyIDResolving(void);


#endif
