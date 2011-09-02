// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  cubepmFactory.c
 * @ingroup  libutilFilesCubePM
 * @brief  Implements the factory functions for CubePM objects.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "cubepmFactory.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Gets extra header information from the file for the CubePM
 *         object.
 *
 * @param[in,out]  cubepm
 *                   The CubePM file object to set extra header
 *                   information for.  PAssing @c NULL is undefined.
 * @param[in,out]  ini
 *                   The INI file from which to read.  Passing @c NULL
 *                   is undefined.
 * @param[in]      sectionName
 *                   The name of the section in the INI file from which
 *                   to read.  This must be a valid, non-empty string.
 *
 * @return  Returns nothing.
 */
static void
local_fillExtrasFromIni(cubepm_t    cubepm,
                        parse_ini_t ini,
                        const char  *sectionName);


/*--- Implementations of exported functions -----------------------------*/
extern cubepm_t
cubepmFactory_fromIni(parse_ini_t ini, const char *sectionName)
{
	cubepm_t cubepm;
	char     *path;
	char     *stem;
	int      nodesDim;
	int      ngrid;

	getFromIni(&path, parse_ini_get_string,
	           ini, "path", sectionName);
	getFromIni(&stem, parse_ini_get_string,
	           ini, "stem", sectionName);
	getFromIni(&nodesDim, parse_ini_get_int32,
	           ini, "nodesDim", sectionName);
	getFromIni(&ngrid, parse_ini_get_int32,
	           ini, "ngrid", sectionName);
	cubepm = cubepm_new(path, stem, nodesDim, ngrid);
	xfree(path);
	xfree(stem);

	local_fillExtrasFromIni(cubepm, ini, sectionName);

	return cubepm;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_fillExtrasFromIni(cubepm_t    cubepm,
                        parse_ini_t ini,
                        const char  *sectionName)
{
	double omega0;
	double lambda0;
	double boxsizeInMpch;
	bool   iniGetCheck;

	iniGetCheck = parse_ini_get_double(ini, "omega0", sectionName,
	                                   &omega0);
	if (iniGetCheck)
		cubepm_setOmega0(cubepm, omega0);

	iniGetCheck = parse_ini_get_double(ini, "lambda0", sectionName,
	                                   &lambda0);
	if (iniGetCheck)
		cubepm_setLambda0(cubepm, lambda0);

	iniGetCheck = parse_ini_get_double(ini, "boxsizeInMpch", sectionName,
	                                   &boxsizeInMpch);
	if (iniGetCheck)
		cubepm_setBoxsizeInMpch(cubepm, boxsizeInMpch);
}
