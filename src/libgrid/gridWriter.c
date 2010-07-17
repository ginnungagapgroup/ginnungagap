// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include <assert.h>
#include <stdio.h>
#ifdef WITH_SILO
#  include "gridWriterSilo.h"
#endif
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridWriter_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridWriter_t
local_newFromIniWrapper(parse_ini_t   ini,
                        gridIO_type_t type,
                        const char    *writerSectionName);


/*--- Implementations of exported functios ------------------------------*/
extern gridWriter_t
gridWriter_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridWriter_t  writer;
	char          *writerTypeName;
	gridIO_type_t type;
	char          *writerSectionName;

	assert(ini != NULL);
	assert(sectionName != NULL);

	getFromIni(&writerTypeName, parse_ini_get_string,
	           ini, "writerType", sectionName);
	type = gridIO_getTypeFromName(writerTypeName);
	xfree(writerTypeName);
	getFromIni(&writerSectionName, parse_ini_get_string,
	           ini, "writerSection", sectionName);

	writer = local_newFromIniWrapper(ini, type, writerSectionName);

	xfree(writerSectionName);

	return writer;
}

extern void
gridWriter_del(gridWriter_t *writer)
{
	assert(writer != NULL && *writer != NULL);

	(*writer)->func->del(writer);
}

extern void
gridWriter_activate(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->func->activate != NULL);

	writer->func->activate(writer);
}

extern void
gridWriter_deactivate(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->func->deactivate != NULL);

	writer->func->deactivate(writer);
}

extern void
gridWriter_writeGridPatch(gridWriter_t   writer,
                          gridPatch_t    patch,
                          const char     *patchName,
                          gridPointDbl_t origin,
                          gridPointDbl_t delta)
{
	assert(writer != NULL);
	assert(patch != NULL);
	assert(patchName != NULL);
	assert(origin != NULL);
	assert(delta != NULL);
	assert(writer->func->writeGridPatch != NULL);

	writer->func->writeGridPatch(writer, patch, patchName, origin, delta);
}

extern void
gridWriter_writeGridRegular(gridWriter_t  writer,
                            gridRegular_t grid)
{
	assert(writer != NULL);
	assert(grid != NULL);
	assert(writer->func->writeGridRegular != NULL);

	writer->func->writeGridRegular(writer, grid);
}

#ifdef WITH_MPI
extern void
gridWriter_initParallel(gridWriter_t writer, MPI_Comm mpiComm)
{
	assert(writer != NULL);
	assert(writer->func->initParallel != NULL);

	writer->func->initParallel(writer, mpiComm);
}

#endif


/*--- Implementations of local functions --------------------------------*/
static gridWriter_t
local_newFromIniWrapper(parse_ini_t   ini,
                        gridIO_type_t type,
                        const char    *writerSectionName)
{
	gridWriter_t writer;

	if (type == IO_TYPE_SILO) {
#ifdef WITH_SILO
		writer = (gridWriter_t)gridWriterSilo_newFromIni(ini,
		                                                 writerSectionName);
#else
		fprintf(stderr,
		        "To use Silo output, run configure using --with-silo\n");
		diediedie(EXIT_FAILURE);
#endif
	} else {
		fprintf(stderr, "Cannot create writer for %s\n",
		        gridIO_getNameFromType(type));
		diediedie(EXIT_FAILURE);
	}

	return writer;
}
