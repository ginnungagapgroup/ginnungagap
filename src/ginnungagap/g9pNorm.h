// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PNORM_H
#define G9PNORM_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pNorm.h
 * @ingroup  ginnungagapNorm
 * @brief  Provides the interface to the normalization routines.
 */


/*--- Includes ----------------------------------------------------------*/
#include "ginnungagapConfig.h"


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Provides the names for the various modes. */
typedef enum {
	/** @brief  No normalization. */
	G9PNORM_MODE_NONE      = 0,
	/** @brief  Adjusts the power spectrum to the provided sigma_8. */
	G9PNORM_MODE_SIGMA8    = 1,
	/** @brief  Uses Anatolys methods (sigma_8 in box). */
	G9PNORM_MODE_SIGMA8BOX = 2,
	/** @brief  Stands for an unknown methods (erroneous to use). */
	G9PNORM_MODE_UNKNOWN   = 3
} g9pNorm_mode_t;

/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Translates a string into a normalization mode.
 *
 * @param[in]  *name
 *                A properly terminated string containing the ASCII name
 *                of the normalization mode.
 *
 * @return  Returns the corresponding mode, or #G9PNORM_UNKNOWN if the
 *          name didn't correspond to a valid mode name.
 */
extern g9pNorm_mode_t
g9pNorm_getModeFromName(const char *name);


/**
 * @brief  Gives the ASCII name of a provided normalization mode.
 *
 * @param[in]  mode
 *
 * @return  Returns a string containing the ASCII name of the provided
 *          mode.  This points to an internal static memory region and
 *          as such can only be used read-only.
 */
extern const char *
g9pNorm_getNameFromMode(g9pNorm_mode_t mode);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup ginnungagapNorm
 * @ingroup ginnungagap
 * @brief Provides various normalization methods for ginnungagap.
 */


#endif
