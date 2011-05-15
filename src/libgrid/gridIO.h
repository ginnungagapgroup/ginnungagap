// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDIO_H
#define GRIDIO_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridIO.h
 * @ingroup libgridIO
 * @brief  Provides the file type definitions and utility functions for
 *         the IO functionality.
 */


/*--- Exported types ----------------------------------------------------*/

/**
 * @brief  Gives the IO type.
 */
typedef enum {
	/** Corresponds to the bov file format. */
	GRIDIO_TYPE_BOV,
	/** Corresponds to the silo file format. */
	GRIDIO_TYPE_SILO,
	/** Corresponds to the grafic file format. */
	GRIDIO_TYPE_GRAFIC,
	/** Stands for an unknown file format. */
	GRIDIO_TYPE_UNKNOWN
} gridIO_type_t;

/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Translates a string into the corresponding file type.
 *
 * @param[in]  *name
 *                A properly terminated string containing the ASCII name
 *                of the file format (the case must match).
 *
 * @return  Returns the corresponding file type or #GRIDIO_TYPE_UNKNOWN
 *          if the name didn't correspond to a valid type name.
 */
extern gridIO_type_t
gridIO_getTypeFromName(const char *name);

/**
 * @brief  Gives the ASCII name of a provided file type.
 *
 * @param[in]  type
 *                The file type that should be translated into an ASCII
 *                string.
 *
 * @return  Returns a string containing the ASCII name of the provided
 *          file type.  This points to an internal static memory region
 *          and as such can only be used read-only.
 */
extern const char *
gridIO_getNameFromType(gridIO_type_t type);

#endif
