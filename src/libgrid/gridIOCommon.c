// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridIOCommon.c
 * @ingroup  libgridIOCommon
 * @brief  Provides common functionality for grid reader and writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridIOCommon.h"
#include "gridIO.h"
#include "../libutil/parse_ini.h"
#include "../libutil/filename.h"
#include "../libutil/xmem.h"
#include <stdlib.h>


/*--- Implementations of exported functions -----------------------------*/
extern gridIO_type_t
gridIOCommon_getType(parse_ini_t ini, const char *secName)
{
	char          *typeName;
	gridIO_type_t type;

	getFromIni(&typeName, parse_ini_get_string, ini, "type", secName);
	type = gridIO_getTypeFromName(typeName);
	xfree(typeName);

	return type;
}

extern filename_t
gridIOCommon_getFileName(parse_ini_t ini, const char *secName, bool allowEmpty)
{
	filename_t fn;
	char       *path      = NULL;
	char       *prefix    = NULL;
	char       *qualifier = NULL;
	char       *suffix    = NULL;

	parse_ini_get_string(ini, "path", secName, &path);
	if (allowEmpty)
		parse_ini_get_string(ini, "prefix", secName, &prefix);
	else
		getFromIni(&prefix, parse_ini_get_string, ini, "prefix", secName);
	parse_ini_get_string(ini, "qualifier", secName, &qualifier);
	parse_ini_get_string(ini, "suffix", secName, &suffix);

	fn = filename_newFull(path, prefix, qualifier, suffix);

	if (prefix != NULL)
		xfree(prefix);
	if (path != NULL)
		xfree(path);
	if (qualifier != NULL)
		xfree(qualifier);
	if (suffix != NULL)
		xfree(suffix);

	return fn;
}

extern bool
gridIOCommon_getOverwrite(parse_ini_t ini, const char *secName)
{
	bool overwrite;

	if (!parse_ini_get_bool(ini, "overwriteFileIfExists", secName,
	                        &overwrite))
		overwrite = false;

	return overwrite;
}
