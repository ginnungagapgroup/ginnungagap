// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReader.c
 * @ingroup libgridIOIn
 * @brief  This file implements the abstract reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "../libutil/parse_ini.h"


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include <assert.h>
#include <stdio.h>
#include "gridReaderBov.h"
#include "gridReaderGrafic.h"
#ifdef WITH_HDF5
#  include "gridReaderHDF5.h"
#endif
#include "gridPatch.h"
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
                        const char    *readerSectionName);


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
gridReader_del(gridReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);

	(*reader)->func->del(reader);
}

extern void
gridReader_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	assert(reader != NULL);
	assert(patch != NULL);
	assert(reader->func->readIntoPatch != NULL);

	reader->func->readIntoPatch(reader, patch);
}

extern void
gridReader_readIntoPatchForVar(gridReader_t reader,
                               gridPatch_t  patch,
                               int          idxOfVar)
{
	assert(reader != NULL);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));

	reader->func->readIntoPatchForVar(reader, patch, idxOfVar);
}

/*--- Implementations of local functions --------------------------------*/
static gridReader_t
local_newFromIniWrapper(parse_ini_t   ini,
                        gridIO_type_t type,
                        const char    *readerSectionName)
{
	gridReader_t r;

	if (type == GRIDIO_TYPE_BOV) {
		r = (gridReader_t)gridReaderBov_newFromIni(ini, readerSectionName);
	} else if (type == GRIDIO_TYPE_GRAFIC) {
		r = (gridReader_t)gridReaderGrafic_newFromIni(ini,
		                                              readerSectionName);
	} else if (type == GRIDIO_TYPE_HDF5) {
#ifdef WITH_HDF5
		r = (gridReader_t)gridReaderHDF5_newFromIni(ini,
		                                            readerSectionName);
#else
		fprintf(stderr,
		        "To use HF5 input, run configure using --with-hdf5\n");
		diediedie(EXIT_FAILURE);
#endif
	} else {
		fprintf(stderr, "Cannot create reader for %s\n",
		        gridIO_getNameFromType(type));
		diediedie(EXIT_FAILURE);
	}

	return r;
}
