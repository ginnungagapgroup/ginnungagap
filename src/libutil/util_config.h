// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef UTIL_CONFIG_H
#define UTIL_CONFIG_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/util_config.h
 * @ingroup libutil
 * @brief This is the main configuration header that all files in this
 *        library include.
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libutil Utility library
 * @brief This library provides various small helper functions and
 *        modules.
 */

/**
 * @defgroup libutilFiles File Interfaces
 * @ingroup libutil
 * @brief Provides low-level interfaces to various file types.
 */

/**
 * @defgroup libutilMisc Miscellaneous
 * @ingroup libutil
 * @brief Provides miscellaneous functions.
 */

/**
 * @defgroup libutilCore Core
 * @ingroup libutil
 * @brief Provides useful functions that are not C99 and wraps around
 *        common functions to die on errors.
 */

/**
 * @defgroup libutilParallel Parallel
 * @ingroup libutil
 * @brief Provides useful things in an MPI environment.
 */

#endif
