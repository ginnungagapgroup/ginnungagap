// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReaderLOI.h"
#include <assert.h>
#include <inttypes.h>
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "lareReader_adt.h"
#include "lareReaderLOI_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct lareReader_func_struct local_func
    = {&lareReaderLOI_del,
	   &lareReaderLOI_read};


/*--- Prototypes of local functions -------------------------------------*/
static void
local_readElementsIntoLare(lare_t lare, FILE *f);


/*--- Implementations of exported functions -----------------------------*/
extern lareReaderLOI_t
lareReaderLOI_newFromIni(parse_ini_t ini, const char *sectionName)
{
	lareReaderLOI_t reader;

	reader       = xmalloc(sizeof(struct lareReaderLOI_struct));
	reader->type = LAREIO_TYPE_LOI;
	reader->func = (lareReader_func_t)&local_func;

	getFromIni(&(reader->fileName), parse_ini_get_string,
	           ini, "fileName", sectionName);
	getFromIni(&(reader->ngrid), parse_ini_get_uint32,
	           ini, "ngrid", sectionName);

	return reader;
}

extern void
lareReaderLOI_del(lareReader_t *reader)
{
	lareReaderLOI_t tmp;

	assert(reader != NULL && *reader != NULL);
	tmp = (lareReaderLOI_t)*reader;
	assert(tmp->type == LAREIO_TYPE_LOI);

	xfree(tmp->fileName);

	xfree(*reader);

	*reader = NULL;
}

extern lare_t
lareReaderLOI_read(lareReader_t reader)
{
	lareReaderLOI_t   tmp;
	lare_t            lare;
	FILE              *f;
	gridPointUint32_t dims;

	assert(reader != NULL);
	assert(reader->type == LAREIO_TYPE_LEGACY);
	tmp = (lareReaderLOI_t)reader;

	f   = xfopen(tmp->fileName, "r");

	for (int i = 0; i < NDIM; i++)
		dims[i] = tmp->ngrid;
	lare = lare_new(dims, 0);

	local_readElementsIntoLare(lare, f);

	xfclose(&f);

	return lare;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_readElementsIntoLare(lare_t lare, FILE *f)
{
	char              *line = NULL;
	size_t            n     = 0;
	gridPointUint32_t element;
	gridPointUint32_t dimsLare;

	lare_getDims(lare, dimsLare);

	while (xgetline(&line, &n, f) != 0) {
		uint64_t id;
		if (line[0] == '#')
			continue;
		sscanf(line, "%" SCNu64, &id);
		for (int i = 0; i < NDIM; i++) {
			element[i] = id % dimsLare[i];
			id        /= dimsLare[i];
		}
		lare_addElement(lare, element);
	}

	xfree(line);
}
