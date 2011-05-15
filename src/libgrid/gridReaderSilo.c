// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderSilo.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "gridPatch.h"
#include "../libdata/dataVar.h"
#include "../libutil/xmem.h"
#include "../libutil/parse_ini.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"
#include "gridReaderSilo_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct gridReader_func_struct local_func
    = { &gridReaderSilo_del,
	    &gridReaderSilo_readIntoPatch,
	    &gridReaderSilo_readIntoPatchForVar };

/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridReaderSilo_t
gridReaderSilo_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReaderSilo_t reader;
	char             *siloFileName;

	reader       = xmalloc(sizeof(struct gridReaderSilo_struct));
	reader->type = GRIDIO_TYPE_SILO;
	reader->func = (gridReader_func_t)&local_func;

	return reader;
}

extern void
gridReaderSilo_del(gridReader_t *reader)
{
	gridReaderSilo_t tmp;

	assert(reader != NULL && *reader != NULL);
	tmp = (gridReaderSilo_t)*reader;
	assert(tmp->type == GRIDIO_TYPE_SILO);

	xfree(*reader);

	*reader = NULL;
}

extern void
gridReaderSilo_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	assert(reader->type == GRIDIO_TYPE_SILO);
	assert(patch != NULL);
}

extern void
gridReaderSilo_readIntoPatchForVar(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar)
{
	assert(reader->type == GRIDIO_TYPE_SILO);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));
}

/*--- Implementations of local functions --------------------------------*/
