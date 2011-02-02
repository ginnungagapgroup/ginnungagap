// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReader.h"
#include "lare.h"
#include "lareIO.h"
#include "lareReaderLegacy.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/parse_ini.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "lareReader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static lareReader_t
local_newFromIniWrapper(parse_ini_t   ini,
                        lareIO_type_t type,
                        const char    *secName);


/*--- Implementations of exported functios ------------------------------*/
extern lareReader_t
lareReader_newFromIni(parse_ini_t ini, const char *sectionName)
{
	lareReader_t  reader;
	char          *readerTypeName;
	lareIO_type_t type;
	char          *readerSectionName;

	assert(ini != NULL);
	assert(sectionName != NULL);

	getFromIni(&readerTypeName, parse_ini_get_string,
	           ini, "readerType", sectionName);
	type = lareIO_getTypeFromName(readerTypeName);
	xfree(readerTypeName);
	getFromIni(&readerSectionName, parse_ini_get_string,
	           ini, "readerSection", sectionName);

	reader = local_newFromIniWrapper(ini, type, readerSectionName);

	xfree(readerSectionName);

	return reader;
}

extern void
lareReader_del(lareReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);
	assert(((*reader)->func != NULL) && ((*reader)->func->del != NULL));

	(*reader)->func->del(reader);
}

extern lare_t
lareReader_read(lareReader_t reader)
{
	assert(reader != NULL);
	assert(((*reader)->func != NULL) && ((*reader)->func->read != NULL));

	return reader->func->read(reader);
}

/*--- Implementations of local functions --------------------------------*/
static lareReader_t
local_newFromIniWrapper(parse_ini_t   ini,
                        lareIO_type_t type,
                        const char    *secName)
{
	lareReader_t reader;

	if (type == LAREIO_TYPE_LEGACY) {
		reader = (lareReader_t)lareReaderLegacy_newFromIni(ini, secName);
	} else {
		fprintf(stderr, "Cannot create lare reader for %s\n",
		        lareIO_getNameFromType(type));
		diediedie(EXIT_FAILURE);
	}

	return reader;
}
