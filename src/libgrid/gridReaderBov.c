// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderBov.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/parse_ini.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"
#include "gridReaderBov_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct gridReader_func_struct local_func = { &gridReaderBov_del };

/*--- Implementations of exported functios ------------------------------*/
extern gridReaderBov_t
gridReaderBov_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReaderBov_t reader;
	char *bovFileName;

	reader       = xmalloc(sizeof(struct gridReaderBov_struct));
	reader->type = IO_TYPE_BOV;
	reader->func = (gridReader_func_t)&local_func;
	
	getFromIni(&bovFileName, parse_ini_get_string,
	           ini, "bovFileName", sectionName);
	reader->bov = bov_newFromFile(bovFileName);
	xfree(bovFileName);

	return reader;
}

extern void
gridReaderBov_del(gridReader_t *reader)
{
	gridReaderBov_t tmp;

	assert(reader != NULL && *reader != NULL);

	tmp = (gridReaderBov_t)*reader;
	bov_del(&(tmp->bov));

	xfree(*reader);

	*reader = NULL;
}

/*--- Implementations of local functions --------------------------------*/
