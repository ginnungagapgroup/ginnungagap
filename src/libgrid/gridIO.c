// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridIO.h"
#include <assert.h>
#include <string.h>


/*--- Local variables ---------------------------------------------------*/
static const char *local_typeBovStr     = "bov";
static const char *local_typeSiloStr    = "silo";
static const char *local_typeUnknownStr = "unknown";


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridIO_type_t
gridIO_getTypeFromName(const char *name)
{
	gridIO_type_t rtn;

	assert(name != NULL);

	if (strcmp(name, local_typeBovStr) == 0)
		rtn = IO_TYPE_BOV;
	else if (strcmp(name, local_typeSiloStr) == 0)
		rtn = IO_TYPE_SILO;
	else
		rtn = IO_TYPE_UNKNOWN;

	return rtn;
}

extern const char *
gridIO_getNameFromType(gridIO_type_t type)
{
	const char *rtn;

	if (type == IO_TYPE_BOV)
		rtn = local_typeBovStr;
	else if (type == IO_TYPE_SILO)
		rtn = local_typeSiloStr;
	else
		rtn = local_typeUnknownStr;

	return rtn;
}

/*--- Implementations of local functions --------------------------------*/
