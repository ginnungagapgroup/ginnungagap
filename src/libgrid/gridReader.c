// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include <assert.h>
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridReader_t
local_newFromIniWrapper(parse_ini_t   ini,
                        gridIO_type_t type,
                        const char    *readerSection);


/*--- Implementations of exported functios ------------------------------*/
extern gridReader_t
gridReader_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReader_t  reader;
	char          *readerTypeName;
	gridIO_type_t type;
	char          *readerSectionName;

	assert(ini != NULL);
	assert(sectionName != NULL);

	getFromIni(&readerTypeName, parse_ini_get_string,
	           ini, "readerType", sectionName);
	type = gridIO_getTypeFromName(readerTypeName);
	xfree(readerTypeName);
	getFromIni(&readerSectionName, parse_ini_get_string,
	           ini, "readerSection", sectionName);

	reader = local_newFromIniWrapper(ini, type, readerSectionName);

	xfree(readerSectionName);

	return reader;
}

extern void
gridReader_del(gridReader_t *gridReader)
{
	assert(gridReader != NULL && *gridReader != NULL);

	*gridReader->func->(*del)(gridReader);
}

/*--- Implementations of local functions --------------------------------*/
static gridReader_t
local_newFromIniWrapper(parse_ini_t   ini,
                        gridIO_type_t type,
                        const char    *readerSection)
{
	gridReader_t reader;

	if (type == IO_TYPE_BOV) {
		reader = (gridReader_t)gridReaderBov_newFromIni(ini,
		                                                readerSectionName);
	} else {
		fprintf(stderr, "Cannot create reader for %s\n", readerTypeName);
		diediedie(EXIT_FAILURE);
	}

	return reader;
}
