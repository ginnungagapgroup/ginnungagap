// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderGrafic.c
 * @ingroup  libgridIOInGrafic
 * @brief  Implements the Grafic reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderGrafic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "gridPatch.h"
#include "../libdata/dataVar.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/grafic.h"
#include "../libutil/parse_ini.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"
#include "gridReaderGrafic_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the functions table for the Grafic reader. */
static struct gridReader_func_struct local_func
    = {&gridReaderGrafic_del,
	   &gridReaderGrafic_readIntoPatch,
	   &gridReaderGrafic_readIntoPatchForVar};

/*--- Prototypes of local functions -------------------------------------*/
static dataVar_t
local_getNewVar(gridReaderGrafic_t grafic);

static graficFormat_t
local_translateGridTypeToGraficType(dataVarType_t type);

static void
local_handleFilenameChange(gridReader_t reader);


/*--- Implementations of abstract functions -----------------------------*/
extern void
gridReaderGrafic_del(gridReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);
	assert((*reader)->type == GRIDIO_TYPE_GRAFIC);

	gridReader_free(*reader);
	gridReaderGrafic_free((gridReaderGrafic_t)*reader);

	xfree(*reader);
	*reader = NULL;
}

extern void
gridReaderGrafic_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	dataVar_t var;
	int       idxOfVar;

	assert(reader->type == GRIDIO_TYPE_GRAFIC);
	assert(patch != NULL);

	var      = local_getNewVar((gridReaderGrafic_t)reader);
	idxOfVar = gridPatch_attachVar(patch, var);

	gridReader_readIntoPatchForVar(reader, patch, idxOfVar);

	dataVar_del(&var);
}

extern void
gridReaderGrafic_readIntoPatchForVar(gridReader_t reader,
                                     gridPatch_t  patch,
                                     int          idxOfVar)
{
	dataVar_t      var;
	dataVarType_t  type;
	void           *data;
	graficFormat_t typeAsGraficType;
	int            numComponents;
	uint32_t       dims[3];
	uint32_t       idxLo[3];

	assert(reader->type == GRIDIO_TYPE_GRAFIC);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));

	var              = gridPatch_getVarHandle(patch, idxOfVar);
	data             = gridPatch_getVarDataHandle(patch, idxOfVar);

	type             = dataVar_getType(var);
	typeAsGraficType = local_translateGridTypeToGraficType(type);
	numComponents    = dataVar_getNumComponents(var);

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);
#if (NDIM == 2)
	idxLo[2] = 0;
	dims[2]  = 1;
#endif

	grafic_readWindowed(((gridReaderGrafic_t)reader)->grafic, data,
	                    typeAsGraficType, numComponents, idxLo, dims);
}

/*--- Implementations of final functions --------------------------------*/
extern gridReaderGrafic_t
gridReaderGrafic_new(void)
{
	gridReaderGrafic_t reader;

	reader = gridReaderGrafic_alloc();

	gridReader_init((gridReader_t)reader, GRIDIO_TYPE_GRAFIC, &local_func,
	                &local_handleFilenameChange);
	gridReaderGrafic_init(reader);

	return reader;
}

extern grafic_t
gridReaderGrafic_getGrafic(const gridReaderGrafic_t reader)
{
	assert(reader != NULL);

	return reader->grafic;
}

/*--- Implementations of protected functions ----------------------------*/
extern gridReaderGrafic_t
gridReaderGrafic_alloc(void)
{
	gridReaderGrafic_t reader;

	reader = xmalloc(sizeof(struct gridReaderGrafic_struct));

	return reader;
}

extern void
gridReaderGrafic_init(gridReaderGrafic_t reader)
{
	reader->grafic = NULL;
}

extern void
gridReaderGrafic_free(gridReaderGrafic_t reader)
{
	if (reader->grafic != NULL)
		grafic_del(&(reader->grafic));
}

extern void
gridReaderGrafic_setGrafic(gridReaderGrafic_t reader, grafic_t grafic)
{
	assert(reader != NULL);

	if (grafic != reader->grafic) {
		if (reader->grafic != NULL)
			grafic_del(&(reader->grafic));
		reader->grafic = grafic;
	}
}

/*--- Implementations of local functions --------------------------------*/
static dataVar_t
local_getNewVar(gridReaderGrafic_t reader)
{
	char      *name;
	dataVar_t var;

	name = xbasename(grafic_getFileName(reader->grafic));
	var  = dataVar_new(name, DATAVARTYPE_FPV, 1);

	xfree(name);

	return var;
}

static graficFormat_t
local_translateGridTypeToGraficType(dataVarType_t type)
{
	graficFormat_t typeAsGraficType;

	if (type == DATAVARTYPE_DOUBLE) {
		typeAsGraficType = GRAFIC_FORMAT_DOUBLE;
	} else if (type == DATAVARTYPE_FPV) {
		if (dataVarType_isNativeFloat(type))
			typeAsGraficType = GRAFIC_FORMAT_FLOAT;
		else
			typeAsGraficType = GRAFIC_FORMAT_DOUBLE;
	} else {
		fprintf(stderr, "Grid type not compatible with grafic type.");
		diediedie(EXIT_FAILURE);
	}
	return typeAsGraficType;
}

static void
local_handleFilenameChange(gridReader_t reader)
{
	assert(reader != NULL);
	assert(reader->type == GRIDIO_TYPE_GRAFIC);

	grafic_t grafic;

	grafic = grafic_newFromFile(filename_getFullName(reader->fileName));
	gridReaderGrafic_setGrafic((gridReaderGrafic_t)reader, grafic);
}
