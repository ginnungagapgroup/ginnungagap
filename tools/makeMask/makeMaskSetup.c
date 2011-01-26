// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "makeMaskSetup.h"
#include <stdlib.h>
#include <assert.h>
#include "../../src/libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "makeMaskSetup_adt.h"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern makeMaskSetup_t
makeMaskSetup_newFromIni(parse_ini_t ini, const char *maskSectionName)
{
	makeMaskSetup_t setup;

	assert(ini != NULL);
	assert(maskSectionName != NULL);

	setup = xmalloc(sizeof(struct makeMaskSetup_struct));

	getFromIni(&(setup->baseGridSize1D), parse_ini_get_uint32,
	           ini, "baseGridSize1D", maskSectionName);
	getFromIni(&(setup->numLevels), parse_ini_get_uint32,
	           ini, "numLevels", maskSectionName);
	getFromIni(&(setup->refinementFactor), parse_ini_get_uint32,
	           ini, "refinementFactor", maskSectionName);
	getFromIni(&(setup->outFileName), parse_ini_get_string,
	           ini, "outFileName", maskSectionName);

	return setup;
}

extern void
makeMaskSetup_del(makeMaskSetup_t *setup)
{
	assert(setup != NULL);
	assert(*setup != NULL);

	if ((*setup)->outFileName != NULL)
		xfree((*setup)->outFileName);
	xfree(*setup);

	*setup = NULL;
}

extern uint32_t
makeMaskSetup_getBaseGridSize1D(const makeMaskSetup_t setup)
{
	assert(setup != NULL);

	return setup->baseGridSize1D;
}


/*--- Implementations of local functions --------------------------------*/
