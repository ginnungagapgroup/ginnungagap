// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridIO.c
 * @ingroup libgridIOTypes
 * @brief  Implements the IO type functions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridIO.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "../libutil/xfile.h"
#include "../libutil/byteswap.h"
#include "../libutil/diediedie.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  The ASCII string for the BOV type. */
static const char *local_typeBovStr     = "bov";

/** @brief  The ASCII string for the Silo type. */
static const char *local_typeSiloStr    = "silo";

/** @brief  The ASCII string for the Grafic type. */
static const char *local_typeGraficStr  = "grafic";

/** @brief  The ASCII string for the HDF5 type. */
static const char *local_typeHDF5Str    = "hdf5";

/** @brief  ASCII string for unknown type. */
static const char *local_typeUnknownStr = "unknown";


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridIO_type_t
gridIO_getTypeFromName(const char *name)
{
	gridIO_type_t rtn;

	assert(name != NULL);

	if (strcmp(name, local_typeBovStr) == 0)
		rtn = GRIDIO_TYPE_BOV;
	else if (strcmp(name, local_typeSiloStr) == 0)
		rtn = GRIDIO_TYPE_SILO;
	else if (strcmp(name, local_typeGraficStr) == 0)
		rtn = GRIDIO_TYPE_GRAFIC;
	else if (strcmp(name, local_typeHDF5Str) == 0)
		rtn = GRIDIO_TYPE_HDF5;
	else
		rtn = GRIDIO_TYPE_UNKNOWN;

	return rtn;
}

extern const char *
gridIO_getNameFromType(gridIO_type_t type)
{
	const char *rtn;

	if (type == GRIDIO_TYPE_BOV)
		rtn = local_typeBovStr;
	else if (type == GRIDIO_TYPE_SILO)
		rtn = local_typeSiloStr;
	else if (type == GRIDIO_TYPE_GRAFIC)
		rtn = local_typeGraficStr;
	else if (type == GRIDIO_TYPE_HDF5)
		rtn = local_typeHDF5Str;
	else
		rtn = local_typeUnknownStr;

	return rtn;
}

/*--- Implementations of local functions --------------------------------*/
