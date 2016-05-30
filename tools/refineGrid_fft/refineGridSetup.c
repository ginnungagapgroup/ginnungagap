// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file refineGrid/refineGridSetup.c
 * @ingroup  toolsrefineGridSetup
 * @brief  Provides the implementation of the setup of the
 *         refineGrid tool.
 */

/*--- Includes ----------------------------------------------------------*/
#include "refineGridConfig.h"
#include "refineGridSetup.h"
#include <stdlib.h>
#include <assert.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/liblare/lare.h"
#include "../../src/liblare/lareReader.h"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern refineGridSetup_t
refineGridSetup_newFromIni(parse_ini_t ini,
                                     const char  *sectionName)
{
	refineGridSetup_t setup;

	assert(ini != NULL);
	assert(sectionName != NULL);

	setup = xmalloc(sizeof(struct refineGridSetup_struct));

	getFromIni(&(setup->boxsizeInMpch), parse_ini_get_double,
	           ini, "boxsizeInMpch", sectionName);
	getFromIni(&(setup->inputDim1D), parse_ini_get_uint32,
	           ini, "inputDim1D", sectionName);
	getFromIni(&(setup->outputDim1D), parse_ini_get_uint32,
	           ini, "outputDim1D", sectionName);
	getFromIni(&(setup->readerSecName), parse_ini_get_string,
		           ini, "readerSecName", sectionName);
	getFromIni(&(setup->writerSecName), parse_ini_get_string,
	           ini, "writerSecName", sectionName);
	getFromIni(&(setup->varName), parse_ini_get_string,
	           ini, "varName", sectionName);
    getFromIni(&(setup->addFields), parse_ini_get_bool,
	           ini, "addFields", sectionName);
    if (!parse_ini_get_bool(ini, "doPk", sectionName, &(setup->doPk))) {              \
    		setup->doPk = false;                                    \
    	}
    if(setup->doPk) {
    	if (!parse_ini_get_string(ini, "PkFile", sectionName, &(setup->PkFile))) {              \
    			setup->PkFile = "Pk_ref.dat";                                    \
    		}
    }
	if(setup->addFields) {
	      getFromIni(&(setup->reader2SecName), parse_ini_get_string,
		           ini, "readerAddSecName", sectionName);           
	} else {
		setup->reader2SecName = NULL;
	}

	return setup;
} /* refineGridSetup_newFromIni */

extern void
refineGridSetup_del(refineGridSetup_t *setup)
{
	assert(setup != NULL);
	assert(*setup != NULL);

	if ((*setup)->writerSecName != NULL)
		xfree((*setup)->writerSecName);
	if ((*setup)->readerSecName != NULL)
		xfree((*setup)->readerSecName);
	if ((*setup)->reader2SecName != NULL)
		xfree((*setup)->reader2SecName);
	xfree(*setup);

	*setup = NULL;
}

/*--- Implementations of local functions --------------------------------*/
