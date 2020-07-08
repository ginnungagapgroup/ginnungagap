// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file checkZero/checkZeroSetup.c
 * @ingroup  toolscheckZeroSetup
 * @brief  Provides the implementation of the setup of the
 *         checkZero tool.
 */

/*--- Includes ----------------------------------------------------------*/
#include "checkZeroConfig.h"
#include "checkZeroSetup.h"
#include <stdlib.h>
#include <assert.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/liblare/lare.h"
#include "../../src/liblare/lareReader.h"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern checkZeroSetup_t
checkZeroSetup_newFromIni(parse_ini_t ini,
                                     const char  *sectionName)
{
	checkZeroSetup_t setup;

	assert(ini != NULL);
	assert(sectionName != NULL);

	setup = xmalloc(sizeof(struct checkZeroSetup_struct));

	getFromIni(&(setup->boxsizeInMpch), parse_ini_get_double,
	           ini, "boxsizeInMpch", sectionName);
	getFromIni(&(setup->inputDim1D), parse_ini_get_uint32,
	           ini, "inputDim1D", sectionName);
	getFromIni(&(setup->readerSecName), parse_ini_get_string,
		           ini, "readerSecName", sectionName);
	getFromIni(&(setup->varName), parse_ini_get_string,
	           ini, "varName", sectionName);
    

	return setup;
} /* checkZeroSetup_newFromIni */

extern void
checkZeroSetup_del(checkZeroSetup_t *setup)
{
	assert(setup != NULL);
	assert(*setup != NULL);

	if ((*setup)->readerSecName != NULL)
		xfree((*setup)->readerSecName);
	xfree(*setup);

	*setup = NULL;
}

/*--- Implementations of local functions --------------------------------*/
