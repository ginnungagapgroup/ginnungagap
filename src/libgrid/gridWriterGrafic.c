// Copyright (C) 2010, 2012 Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterGrafic.c
 * @ingroup  libgridIOOutGrafic
 * @brief  Implements the Grafc writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterGrafic.h"
#include <assert.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#  include "../libutil/groupi.h"
#endif
#include "../libdata/dataVar.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridWriterGrafic_adt.h"


/*--- Local defines -----------------------------------------------------*/

#ifdef WITH_MPI
/** @brief  The tag used for MPI messages in the parallel IO communication. */
#  define LOCAL_MPI_TAG 987
#endif


/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the functions table for the Grafic writer. */
static struct gridWriter_func_struct local_func
    = {&gridWriterGrafic_del,
	   &gridWriterGrafic_activate,
	   &gridWriterGrafic_deactivate,
	   &gridWriterGrafic_writeGridPatch,
	   &gridWriterGrafic_writeGridRegular,
#ifdef WITH_MPI
	   &gridWriterGrafic_initParallel
#endif
	};

/** @brief  Gives the allowed suffices for Grafic fiels. */

/** @brief  Gives the default path of the output file. */
static const char *local_defaultFileNamePath = NULL;

/** @brief  Gives the default prefix of the output file. */
static const char *local_defaultFileNamePrefix = "ic";

/** @brief  Gives the default suffix for the output file. */
static const char *local_defaultFileNameSuffix = NULL;

/** @brief  Gives the default qualifier of the output file. */
static const char *local_defaultFileNameQualifier = "_delta";


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Translates the grid variable type to a Grafic variable type.
 *
 * @param[in]  var
 *                The variable that should be translated.
 *
 * @return  Returns the corresponding Grafic variable.
 */
static graficFormat_t
local_getGraficTypeFromGridType(const dataVar_t var);


/*--- Implementations of abstract functions -----------------------------*/
extern void
gridWriterGrafic_del(gridWriter_t *writer)
{
	assert(writer != NULL && *writer != NULL);
	assert((*writer)->type == GRIDIO_TYPE_GRAFIC);


	gridWriter_free(*writer);
	gridWriterGrafic_free((gridWriterGrafic_t)*writer);

	xfree(*writer);
	*writer = NULL;

	xfree(*writer);

	*writer = NULL;
}

extern void
gridWriterGrafic_activate(gridWriter_t writer)
{
	gridWriterGrafic_t w = (gridWriterGrafic_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_GRAFIC);
#ifdef WITH_MPI
	assert(w->groupi != NULL);
#endif


	if (!gridWriter_isActive(writer)) {
		bool isFirst = true;

#ifdef WITH_MPI
		groupi_acquire(w->groupi);
		isFirst = groupi_isFirstInGroup(w->groupi);
#endif
		grafic_setFileName(w->grafic, filename_getFullName(w->base.fileName));
		if (isFirst)
			grafic_makeEmptyFile(w->grafic);

		gridWriter_setIsActive(writer);
	}
}

extern void
gridWriterGrafic_deactivate(gridWriter_t writer)
{
	gridWriterGrafic_t w = (gridWriterGrafic_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_GRAFIC);
#ifdef WITH_MPI
	assert(w->groupi != NULL);
#endif

	if (gridWriter_isActive(writer)) {
#ifdef WITH_MPI
		groupi_release(w->groupi);
#endif
		gridWriter_setIsInactive(writer);
	}
}

extern void
gridWriterGrafic_writeGridPatch(gridWriter_t   writer,
                                gridPatch_t    patch,
                                const char     *patchName,
                                gridPointDbl_t origin,
                                gridPointDbl_t delta)
{
	gridWriterGrafic_t w = (gridWriterGrafic_t)writer;
	void               *data;
	dataVar_t          var;
	int                numComponents;
	gridPointUint32_t  dims;
	gridPointUint32_t  idxLo;
	graficFormat_t     format;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_GRAFIC);
	assert(gridWriter_isActive(writer));
	assert(w->grafic != NULL);
	assert(patch != NULL);
	assert(patchName != NULL);

	if ((patchName == NULL) || (origin == NULL) || (delta == NULL))
		;

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);
	var           = gridPatch_getVarHandle(patch, 0);
	data          = gridPatch_getVarDataHandle(patch, 0);
	numComponents = dataVar_getNumComponents(var);
	format        = local_getGraficTypeFromGridType(var);

	grafic_writeWindowed(w->grafic, data, format, numComponents,
	                     idxLo, dims);
}

extern void
gridWriterGrafic_writeGridRegular(gridWriter_t  writer,
                                  gridRegular_t grid)
{
	gridPatch_t patch;

	assert(writer != NULL);
	assert(writer->type == GRIDIO_TYPE_GRAFIC);
	assert(writer->isActive);
	assert(grid != NULL);

	patch = gridRegular_getPatchHandle(grid, 0);
	gridWriterGrafic_writeGridPatch(writer, patch, "null",
	                                NULL, NULL);
}

#ifdef WITH_MPI
extern void
gridWriterGrafic_initParallel(gridWriter_t writer, MPI_Comm mpiComm)
{
	gridWriterGrafic_t tmp = (gridWriterGrafic_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_GRAFIC);

	tmp->groupi = groupi_new(1, mpiComm, LOCAL_MPI_TAG,
	                         GROUPI_MODE_BLOCK);
}

#endif


/*--- Implementations of final functions --------------------------------*/
extern gridWriterGrafic_t
gridWriterGrafic_new(void)
{
	gridWriterGrafic_t writer;

	writer = gridWriterGrafic_alloc();

	gridWriter_init((gridWriter_t)writer, GRIDIO_TYPE_GRAFIC, &local_func);
	gridWriterGrafic_init(writer);

	return writer;
}

extern void
gridWriterGrafic_setGrafic(gridWriterGrafic_t writer, grafic_t grafic)
{
	assert(writer != NULL);
	assert(grafic != NULL);

	if (grafic != writer->grafic) {
		grafic_del(&(writer->grafic));
		writer->grafic = grafic;
	}
}

extern grafic_t
gridWriterGrafic_getGrafic(const gridWriterGrafic_t writer)
{
	assert(writer != NULL);

	return writer->grafic;
}

/*--- Implementations of protected functions ----------------------------*/
extern gridWriterGrafic_t
gridWriterGrafic_alloc(void)
{
	gridWriterGrafic_t writer;

	writer = xmalloc(sizeof(struct gridWriterGrafic_struct));

	return writer;
}

extern void
gridWriterGrafic_init(gridWriterGrafic_t writer)
{
	gridWriter_setFileName((gridWriter_t)writer,
	                       filename_newFull(local_defaultFileNamePath,
	                                        local_defaultFileNamePrefix,
	                                        local_defaultFileNameQualifier,
	                                        local_defaultFileNameSuffix));

	writer->grafic = grafic_new();
#ifdef WITH_MPI
	writer->groupi = NULL;
#endif
}

extern void
gridWriterGrafic_free(gridWriterGrafic_t writer)
{
	if (writer->grafic != NULL)
		grafic_del(&(writer->grafic));
#ifdef WITH_MPI
	if (writer->groupi != NULL)
		groupi_del(&(writer->groupi));
#endif
}

/*--- Implementations of local functions --------------------------------*/
static graficFormat_t
local_getGraficTypeFromGridType(const dataVar_t var)
{
	graficFormat_t varType;

	switch (dataVar_getType(var)) {
	case DATAVARTYPE_DOUBLE:
		varType = GRAFIC_FORMAT_DOUBLE;
		break;
	case DATAVARTYPE_FPV:
		varType = sizeof(fpv_t) == 4 ?
		          GRAFIC_FORMAT_FLOAT : GRAFIC_FORMAT_DOUBLE;
		break;
	case DATAVARTYPE_INT:
	default:
		diediedie(EXIT_FAILURE);
	}

	return varType;
}
