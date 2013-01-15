// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef G9PFIELDID_H
#define G9PFIELDID_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pFieldID.h
 * @ingroup  libg9pFieldID
 * @brief  Provides the interface to field IDs.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"


/*--- Typedefs ----------------------------------------------------------*/

typedef enum {
	/** @brief  For white noise fields. */
	G9PFIELDID_WN = 0,
	/** @brief  For overdensity fields. */
	G9PFIELDID_DELTA = 1,
	/** @brief  For the x-component of the velocity. */
	G9PFIELDID_VX = 2,
	/** @brief  For the y-component of the velocity. */
	G9PFIELDID_VY = 3,
	/** @brief  For the z-component of the velocity. */
	G9PFIELDID_VZ = 4
} g9pFieldID_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Translates a field id to a string.
 *
 * @param[in]  fid
 *                The field ID to translate.
 *
 * @return  Returns the string representation of the provided field id.
 *          This is a pointer to private memory region and thus may only be
 *          used read-only.
 */
extern const char *
g9pFieldID_getStr(const g9pFieldID_t fid);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libg9pFieldID Field ID
 * @ingroup libg9p
 * @brief Provides a type to identify fields.
 */

#endif
