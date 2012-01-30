// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriter.c
 * @ingroup libgridIOOut
 * @brief  This file provides the implementations of the abstract grid
 *         writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include <assert.h>
#include <stdio.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "../libutil/filename.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridWriter_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of virtual function -------------------------------*/
extern void
gridWriter_del(gridWriter_t *writer)
{
	assert(writer != NULL && *writer != NULL);
	assert((*writer)->func->del != NULL);

	(*writer)->func->del(writer);
}

extern void
gridWriter_activate(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->func->activate != NULL);

	writer->func->activate(writer);
	writer->hasBeenActivated = true;
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


/*--- Implementations of final functions --------------------------------*/
extern void
gridWriter_setOverwriteFileIfExists(gridWriter_t writer,
                                    bool         overwriteFileIfExists)
{
	assert(writer != NULL);

	writer->overwriteFileIfExists = overwriteFileIfExists;
}

extern bool
gridWriter_getOverwriteFileIfExists(const gridWriter_t writer)
{
	assert(writer != NULL);

	return writer->overwriteFileIfExists;
}

extern void
gridWriter_setFileName(gridWriter_t writer,
                       filename_t   fileName)
{
	assert(writer != NULL);
	assert(fileName != NULL);
	assert(!gridWriter_isActive(writer));

	if (writer->fileName != NULL)
		filename_del(&(writer->fileName));
	writer->fileName = fileName;
	writer->hasBeenActivated = false;
}

extern void
gridWriter_overlayFileName(gridWriter_t writer,
                           const filename_t fileName)
{
	assert(writer != NULL);
	assert(fileName != NULL);
	assert(!gridWriter_isActive(writer));

	if (writer->fileName == NULL) 
		writer->fileName = filename_clone(fileName);
	else 
		filename_copySetFields(writer->fileName, fileName);

	writer->hasBeenActivated = false;
}

extern const filename_t
gridWriter_getFileName(const gridWriter_t writer)
{
	assert(writer != NULL);

	return writer->fileName;
}

extern bool
gridWriter_isActive(const gridWriter_t writer)
{
	assert(writer != NULL);

	return writer->isActive;
}

extern bool
gridWriter_hasBeenActivated(const gridWriter_t writer)
{
	assert(writer != NULL);

	return writer->hasBeenActivated;
}

/*--- Implementations of protected functions ----------------------------*/
extern void
gridWriter_init(gridWriter_t      writer,
                gridIO_type_t     type,
                gridWriter_func_t func)
{
	assert(writer != NULL);

	writer->type                  = type;
	writer->func                  = func;
	writer->isActive              = false;
	writer->hasBeenActivated      = false;
	writer->overwriteFileIfExists = false;
	writer->fileName              = NULL;
}

extern void
gridWriter_free(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->isActive == false);

	if (writer->isActive)
		gridWriter_deactivate(writer);

	if (writer->fileName != NULL)
		filename_del(&(writer->fileName));
}

/*--- Implementations of local functions --------------------------------*/
