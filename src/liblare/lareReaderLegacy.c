// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReaderLegacy.h"
#include <assert.h>
#include <inttypes.h>
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "lareReader_adt.h"
#include "lareReaderLegacy_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct lareReader_func_struct local_func
    = { &lareReaderLegacy_del,
	    &lareReaderLegacy_read };


/*--- Prototypes of local functions -------------------------------------*/
static uint32_t
local_getNgridFromHeader(const char *fileName);

static void
local_skipHeader(FILE *f);

static void
local_readElementsIntoLare(lare_t lare, FILE *f);


/*--- Implementations of exported functions -----------------------------*/
extern lareReaderLegacy_t
lareReaderLegacy_newFromIni(parse_ini_t ini, const char *sectionName)
{
	lareReaderLegacy_t reader;

	reader       = xmalloc(sizeof(struct lareReaderLegacy_struct));
	reader->type = LAREIO_TYPE_LEGACY;
	reader->func = (lareReader_func_t)&local_func;

	getFromIni(&(reader->fileName), parse_ini_get_string,
	           ini, "fileName", sectionName);
	getFromIni(&(reader->hasHeader), parse_ini_get_bool,
	           ini, "hasHeader", sectionName);
	if (reader->hasHeader)
		reader->ngrid = local_getNgridFromHeader(reader->fileName);
	else
		getFromIni(&(reader->ngrid), parse_ini_get_uint32,
		           ini, "ngrid", sectionName);

	return reader;
}

extern void
lareReaderLegacy_del(lareReader_t *reader)
{
	lareReaderLegacy_t tmp;

	assert(reader != NULL && *reader != NULL);
	tmp = (lareReaderLegacy_t)*reader;
	assert(tmp->type == LAREIO_TYPE_LEGACY);

	xfree(tmp->fileName);

	xfree(*reader);

	*reader = NULL;
}

extern lare_t
lareReaderLegacy_read(lareReader_t reader)
{
	lareReaderLegacy_t tmp;
	lare_t             lare;
	FILE               *f;
	gridPointUint32_t  dims;

	assert(reader != NULL);
	assert(reader->type == LAREIO_TYPE_LEGACY);
	tmp = (lareReaderLegacy_t)reader;

	f   = xfopen(tmp->fileName, "r");

	for (int i = 0; i < NDIM; i++)
		dims[i] = tmp->ngrid;
	lare = lare_new(dims, 0);

	if (tmp->hasHeader)
		local_skipHeader(f);
	local_readElementsIntoLare(lare, f);

	xfclose(&f);

	return lare;
}

/*--- Implementations of local functions --------------------------------*/
static uint32_t
local_getNgridFromHeader(const char *fileName)
{
	uint32_t ngrid;
	char     *line = NULL;
	size_t   n     = 0;
	FILE     *f;

	f = xfopen(fileName, "r");

	xgetline(&line, &n, f);
	xgetline(&line, &n, f);
	xgetline(&line, &n, f);

	sscanf(line, " Nrow= %*i Ngrid= %" SCNu32, &ngrid);

	xfree(line);

	xfclose(&f);

	return ngrid;
}

static void
local_skipHeader(FILE *f)
{
	char   *line = NULL;
	size_t n     = 0;

	xgetline(&line, &n, f);
	xgetline(&line, &n, f);
	xgetline(&line, &n, f);
	xgetline(&line, &n, f);
	xgetline(&line, &n, f);

	xfree(line);
}

static void
local_readElementsIntoLare(lare_t lare, FILE *f)
{
	char              *line = NULL;
	size_t            n     = 0;
	gridPointUint32_t element;

	while (xgetline(&line, &n, f) != 0) {
		uint32_t ijk[3];
		sscanf(line, "%*i %*e %*e %*e %*e %*e %*e"
		             " %" SCNu32 " %" SCNu32 "%" SCNu32,
		       ijk, ijk + 1, ijk + 2);
		for (int i = 0; i < NDIM; i++)
			element[i] = ijk[i] - 1; // Fortran to proper, hence the -1
		lare_addElement(lare, element);
	}

	xfree(line);
}
