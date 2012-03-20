// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReader.c
 * @ingroup libgridIOInInterface
 * @brief  This file implements the abstract reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReader.h"
#include <assert.h>
#include <stdio.h>
#include "gridPatch.h"
#include "../libutil/xmem.h"
#include "../libutil/filename.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of virtual functions ------------------------------*/
extern void
gridReader_del(gridReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);
	assert((*reader)->func->del != NULL);

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

/*--- Implementations of final functions --------------------------------*/
extern void
gridReader_setFileName(gridReader_t reader,
                       filename_t   fileName)
{
	assert(reader != NULL);
	assert(fileName != NULL);

	if (reader->fileName != NULL)
		filename_del(&(reader->fileName));
	reader->fileName = fileName;

	if (reader->handleFilenameChange != NULL)
		reader->handleFilenameChange(reader);
}

extern void
gridReader_overlayFileName(gridReader_t     reader,
                           const filename_t fileName)
{
	assert(reader != NULL);
	assert(fileName != NULL);

	if (reader->fileName == NULL)
		reader->fileName = filename_clone(fileName);
	else
		filename_copySetFields(reader->fileName, fileName);

	if (reader->handleFilenameChange != NULL)
		reader->handleFilenameChange(reader);
}

extern const filename_t
gridReader_getFileName(const gridReader_t reader)
{
	assert(reader != NULL);

	return reader->fileName;
}

/*--- Implementations of protected functions ----------------------------*/
extern void
gridReader_init(gridReader_t                          reader,
                gridIO_type_t                         type,
                gridReader_func_t                     func,
                gridReader_handleFilenameChangeFunc_t handleFilenameChange)
{
	assert(reader != NULL);

	reader->type                 = type;
	reader->func                 = func;
	reader->handleFilenameChange = handleFilenameChange;
	reader->fileName             = NULL;
}

extern void
gridReader_free(gridReader_t reader)
{
	assert(reader != NULL);

	if (reader->fileName != NULL)
		filename_del(&(reader->fileName));
}

/*--- Implementations of local functions --------------------------------*/
