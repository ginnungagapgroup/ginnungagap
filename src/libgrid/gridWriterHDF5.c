// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterHDF5.c
 * @ingroup  libgridIOOutHDF5
 * @brief  Implements the HDF5 writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterHDF5.h"
#include <assert.h>
#include "gridRegular.h"
#include "gridPatch.h"
#include "gridPoint.h"
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "gridWriterHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct gridWriter_func_struct local_func
    = {&gridWriterHDF5_del,
	   &gridWriterHDF5_activate,
	   &gridWriterHDF5_deactivate,
	   &gridWriterHDF5_writeGridPatch,
	   &gridWriterHDF5_writeGridRegular,
#ifdef WITH_MPI
	   &gridWriterHDF5_initParallel
#endif
	};

/*--- Implementations of exported functions -----------------------------*/
extern gridWriterHDF5_t
gridWriterHDF5_new(void)
{
	gridWriterHDF5_t writer;

	writer = xmalloc(sizeof(struct gridWriterHDF5_struct));
	writer->type = GRIDIO_TYPE_HDF5;
	writer->func = (gridWriter_func_t)&local_func;

	return writer;
}

extern gridWriterHDF5_t
gridWriterHDF5_newFromIni(parse_ini_t ini, const char *sectionName)
{
	assert(ini != NULL);
	assert(sectionName != NULL);

	gridWriterHDF5_t writer;

	writer = gridWriterHDF5_new();

	return writer;
}

extern void
gridWriterHDF5_del(gridWriter_t *writer)
{
	assert(writer != NULL && *writer != NULL);
	gridWriterHDF5_t tmp = (gridWriterHDF5_t)*writer;
	assert(tmp->type == GRIDIO_TYPE_HDF5);

	xfree(*writer);

	*writer = NULL;
}

extern void
gridWriterHDF5_activate(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->type == GRIDIO_TYPE_HDF5);

	;
}

extern void
gridWriterHDF5_deactivate(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->type == GRIDIO_TYPE_HDF5);

	;
}

extern void
gridWriterHDF5_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta)
{
	gridWriterHDF5_t tmp = (gridWriterHDF5_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_HDF5);
	assert(patch != NULL);
	assert(patchName != NULL);
}

extern void
gridWriterHDF5_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid)
{
	gridWriterHDF5_t tmp = (gridWriterHDF5_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_HDF5);
	assert(grid != NULL);
}


#ifdef WITH_MPI
extern void
gridWriterHDF5_initParallel(gridWriter_t writer, MPI_Comm mpiComm)
{
	gridWriterHDF5_t tmp = (gridWriterHDF5_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_HDF5);
}

#endif


/*--- Implementations of local functions --------------------------------*/
