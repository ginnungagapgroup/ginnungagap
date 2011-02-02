// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareIO.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "../libutil/xfile.h"
#include "../libutil/byteswap.h"
#include "../libutil/diediedie.h"


/*--- Local variables ---------------------------------------------------*/
static const char *local_typeLegacyStr  = "legacy";
static const char *local_typeUnknownStr = "unknown";


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern lareIO_type_t
lareIO_getTypeFromName(const char *name)
{
	lareIO_type_t rtn;

	assert(name != NULL);

	if (strcmp(name, local_typeLegacyStr) == 0)
		rtn = LAREIO_TYPE_LEGACY;
	else
		rtn = LAREIO_TYPE_UNKNOWN;

	return rtn;
}

extern const char *
lareIO_getNameFromType(lareIO_type_t type)
{
	const char *rtn;

	if (type == LAREIO_TYPE_LEGACY)
		rtn = local_typeLegacyStr;
	else
		rtn = local_typeUnknownStr;

	return rtn;
}

/*--- Implementations of local functions --------------------------------*/
