// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOTF_TESTS_H
#define COSMOTF_TESTS_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libcosmo/cosmoTF_tests.h
 * @ingroup  libcosmoTF
 * @brief  Provides tests for the transfer functions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
cosmoTF_eisensteinHu1998_test(void);

extern bool
cosmoTF_scaleFree_test(void);


#endif
