// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderHDF5.c
 * @ingroup  libgridIOInHDF5
 * @brief  Implements the HDF5 reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderHDF5.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Implementation of main structure ----------------------------------*/
#include "gridReaderHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct gridReader_func_struct local_func
    = {&gridReaderHDF5_del,
	   &gridReaderHDF5_readIntoPatch,
	   &gridReaderHDF5_readIntoPatchForVar};

/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern gridReaderHDF5_t
gridReaderHDF5_new(void)
{
	gridReaderHDF5_t reader;

	reader = xmalloc(sizeof(struct gridReaderHDF5_struct));
	reader->type = GRIDIO_TYPE_HDF5;
	reader->func = (gridReader_func_t)&local_func;

	return reader;
}

extern void
gridReaderHDF5_del(gridReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);

	xfree(*reader);

	*reader = NULL;
}

extern void
gridReaderHDF5_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	assert(reader != NULL);
	assert(reader->type = GRIDIO_TYPE_HDF5);
	assert(patch != NULL);
}

extern void
gridReaderHDF5_readIntoPatchForVar(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar)
{
	assert(reader != NULL);
	assert(reader->type = GRIDIO_TYPE_HDF5);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));
}

/*--- Implementations of local functions --------------------------------*/
