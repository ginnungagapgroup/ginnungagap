// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file realSpaceConstraints/realSpaceConstraintsSetup.c
 * @ingroup  toolsRealSpaceConstraintsSetup
 * @brief  Provides the implementation of the setup of the
 *         realSpaceConstraints tool.
 */

/*--- Includes ----------------------------------------------------------*/
#include "realSpaceConstraintsConfig.h"
#include "realSpaceConstraintsSetup.h"
#include <stdlib.h>
#include <assert.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/liblare/lare.h"
#include "../../src/liblare/lareReader.h"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern realSpaceConstraintsSetup_t
realSpaceConstraintsSetup_newFromIni(parse_ini_t ini,
                                     const char  *sectionName)
{
	realSpaceConstraintsSetup_t setup;

	assert(ini != NULL);
	assert(sectionName != NULL);

	setup = xmalloc(sizeof(struct realSpaceConstraintsSetup_struct));

	getFromIni(&(setup->boxsizeInMpch), parse_ini_get_double,
	           ini, "boxsizeInMpch", sectionName);
	getFromIni(&(setup->inputDim1D), parse_ini_get_uint32,
	           ini, "inputDim1D", sectionName);
	getFromIni(&(setup->outputDim1D), parse_ini_get_uint32,
	           ini, "outputDim1D", sectionName);
	getFromIni(&(setup->useFileForInput), parse_ini_get_bool,
	           ini, "useFileForInput", sectionName);
	getFromIni(&(setup->seedIn), parse_ini_get_int32,
	           ini, "seedIn", sectionName);
	getFromIni(&(setup->seedOut), parse_ini_get_int32,
	           ini, "seedOut", sectionName);
	if (setup->useFileForInput) {
		getFromIni(&(setup->readerSecName), parse_ini_get_string,
		           ini, "readerSecName", sectionName);
		setup->writerInSecName = NULL;
	} else {
		setup->readerSecName = NULL;
		getFromIni(&(setup->writerInSecName), parse_ini_get_string,
		           ini, "writerInSecName", sectionName);
	}
	getFromIni(&(setup->writerSecName), parse_ini_get_string,
	           ini, "writerSecName", sectionName);

	return setup;
} /* realSpaceConstraintsSetup_newFromIni */

extern void
realSpaceConstraintsSetup_del(realSpaceConstraintsSetup_t *setup)
{
	assert(setup != NULL);
	assert(*setup != NULL);

	if ((*setup)->writerSecName != NULL)
		xfree((*setup)->writerSecName);
	if ((*setup)->readerSecName != NULL)
		xfree((*setup)->readerSecName);
	if ((*setup)->writerInSecName != NULL)
		xfree((*setup)->writerInSecName);
	xfree(*setup);

	*setup = NULL;
}

/*--- Implementations of local functions --------------------------------*/
