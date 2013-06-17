// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETVERSION_H
#define GADGETVERSION_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetVersion.h
 * @ingroup libutilFilesGadgetVersion
 * @brief  This file provides the interface to utility functions dealing
 *         with the Gadget format versions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>


/*--- Defines -----------------------------------------------------------*/

/** @brief  Gives the number of different versions. */
#define GADGETVERSION_NUM 3


/*--- Typedefs ----------------------------------------------------------*/

/** @brief  Defines various file versions (changing on-disk format). */
typedef enum {
	/// @brief  Flags unknown file versions.
	GADGETVERSION_UNKNOWN = 0,
	/// @brief  This is for version 1 files.
	GADGETVERSION_ONE = 1,
	/// @brief  This is for version 2 files.
	GADGETVERSION_TWO = 2,
} gadgetVersion_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Retrieves the Gadget version from the file.
 *
 * @param[in]  *f
 *                The file pointer from which to infer the Gadget version.
 *                The file must be opened for binary reading, after
 *                completion of the function the file pointer will be
 *                restored to the position it had before entering the
 *                function.  Note that any IO error is cleared.
 *
 * @return  Returns the file version.  If the version could not be
 *          determined, or in IO error occurred, #GADGETVERSION_UNKNOWN is
 *          returned.
 */
extern gadgetVersion_t
gadgetVersion_getVersionFromFile(FILE *f);

/**
 * @brief  Returns a string describing the version.
 *
 * @param[in]  ver
 *                The version that should be converted to a string.
 *
 * @return  A pointer to an internal memory location holding the string
 *          describing the version.
 */
extern const char *
gadgetVersion_getNameFromType(const gadgetVersion_t ver);

/**
 * @brief  Returns the version enum corresponding to a string.
 *
 * @param[in]  name
 *                The name that should be parsed into a version enum.
 *
 * @return  Returns the enum corresponding to the version described by the
 *          string.  This may be #GADGETVERSION_UNKNOWN if the name didn't
 *          match.
 */
extern gadgetVersion_t
gadgetVersion_getTypeFromName(const char *name);


/*--- Doxygen group definition ------------------------------------------*/

/**
 * @defgroup libutilFilesGadgetVersion Gadget File Version
 * @ingroup libutilFilesGadget
 * @brief Provides functionality to deal with Gadget file versions.
 */


#endif
