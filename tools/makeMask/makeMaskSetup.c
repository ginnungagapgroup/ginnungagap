// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "makeMaskSetup.h"
#include <stdlib.h>
#include <assert.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"


/*--- Prototypes of local functions -------------------------------------*/
#ifdef WITH_MPI
static void
local_parseMPIStuff(makeMaskSetup_t setup,
                    parse_ini_t     ini,
                    const char      *maskSectionName);

#endif


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
	if (!parse_ini_get_string(ini, "outSecName", maskSectionName,
	                          &(setup->outSecName)))
		setup->outSecName = xstrdup(MAKEMASK_SECTIONNAME_WRITER);
#ifdef WITH_MPI
	local_parseMPIStuff(setup, ini, maskSectionName);
#endif

	return setup;
}

extern void
makeMaskSetup_del(makeMaskSetup_t *setup)
{
	assert(setup != NULL);
	assert(*setup != NULL);

	if ((*setup)->outFileName != NULL)
		xfree((*setup)->outFileName);
	if ((*setup)->outSecName != NULL)
		xfree((*setup)->outSecName);
	xfree(*setup);

	*setup = NULL;
}

/*--- Implementations of local functions --------------------------------*/
#ifdef WITH_MPI
static void
local_parseMPIStuff(makeMaskSetup_t setup,
                    parse_ini_t     ini,
                    const char      *maskSectionName)
{
	int32_t *nProcs;
	bool    rtn;

	rtn = parse_ini_get_int32list(ini, "nProcs", maskSectionName,
	                              NDIM, &nProcs);
	if (!rtn) {
		fprintf(stderr, "Could not get nProcs from section MPI.\n");
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < NDIM; i++)
		setup->nProcs[i] = (int)(nProcs[i]);
	xfree(nProcs);
}
#endif
