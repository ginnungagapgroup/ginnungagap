// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderBov.c
 * @ingroup  libgridIOInBov
 * @brief  Implements the BOV reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderBov.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "gridPatch.h"
#include "../libdata/dataVar.h"
#include "../libutil/xmem.h"
#include "../libutil/parse_ini.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"
#include "gridReaderBov_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the functions table for the BOV reader. */
static struct gridReader_func_struct local_func
    = { &gridReaderBov_del,
	    &gridReaderBov_readIntoPatch,
	    &gridReaderBov_readIntoPatchForVar };

/*--- Prototypes of local functions -------------------------------------*/
static dataVar_t
local_getNewVar(bov_t bov);

static dataVarType_t
local_translateBovTypeToGridType(bovFormat_t typeInBov);

static bovFormat_t
local_translateGridTypeToBovType(dataVarType_t type);

static void
local_handleFilenameChange(gridReader_t reader);


/*--- Implementations of exported functios ------------------------------*/
extern void
gridReaderBov_del(gridReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);
	assert((*reader)->type == GRIDIO_TYPE_BOV);

	gridReader_free(*reader);
	gridReaderBov_free((gridReaderBov_t)*reader);

	xfree(*reader);
	*reader = NULL;
}

extern void
gridReaderBov_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	dataVar_t       var;
	int             idxOfVar;

	assert(reader->type == GRIDIO_TYPE_BOV);
	assert(patch != NULL);

	var      = local_getNewVar(((gridReaderBov_t)reader)->bov);
	idxOfVar = gridPatch_attachVar(patch, var);

	gridReader_readIntoPatchForVar(reader, patch, idxOfVar);

	dataVar_del(&var);
}

extern void
gridReaderBov_readIntoPatchForVar(gridReader_t reader,
                                  gridPatch_t  patch,
                                  int          idxOfVar)
{
	dataVar_t     var;
	dataVarType_t type;
	void          *data;
	bovFormat_t   typeAsBovType;
	int           numComponents;
	uint32_t      dims[3];
	uint32_t      idxLo[3];

	assert(reader->type == GRIDIO_TYPE_BOV);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));

	var           = gridPatch_getVarHandle(patch, idxOfVar);
	data          = gridPatch_getVarDataHandle(patch, idxOfVar);

	type          = dataVar_getType(var);
	typeAsBovType = local_translateGridTypeToBovType(type);
	numComponents = dataVar_getNumComponents(var);

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);
#if (NDIM == 2)
	idxLo[2] = 0;
	dims[0]  = 1;
#endif

	bov_readWindowed(((gridReaderBov_t)reader)->bov, data, typeAsBovType,
	                 numComponents, idxLo, dims);
}

/*--- Implementations of final functions --------------------------------*/
extern gridReaderBov_t
gridReaderBov_new(void)
{
	gridReaderBov_t reader = gridReaderBov_alloc();

	gridReader_init((gridReader_t)reader, GRIDIO_TYPE_BOV, &local_func,
	                &local_handleFilenameChange);
	gridReaderBov_init(reader);

	return reader;
}

extern bov_t
gridReaderBov_getBov(const gridReaderBov_t reader)
{
	assert(reader != NULL);

	return reader->bov;
}

/*--- Implementations of protected functions ----------------------------*/
extern gridReaderBov_t
gridReaderBov_alloc(void)
{
	return (gridReaderBov_t)xmalloc(sizeof(struct gridReaderBov_struct));
}

extern void
gridReaderBov_init(gridReaderBov_t reader)
{
	reader->bov = NULL;
}

extern void
gridReaderBov_free(gridReaderBov_t reader)
{
	if (reader->bov != NULL)
		bov_del(&(reader->bov));
}

extern void
gridReaderBov_setBov(gridReaderBov_t reader, bov_t bov)
{
	assert(reader != NULL);

	if (bov != reader->bov) {
		if (reader->bov != NULL)
			bov_del(&(reader->bov));
		reader->bov = bov;
	}
}

/*--- Implementations of local functions --------------------------------*/
static dataVar_t
local_getNewVar(bov_t bov)
{
	char          *name;
	dataVarType_t type;
	bovFormat_t   typeInBov;
	int           numComponents;
	dataVar_t     var;

	name          = bov_getVarName(bov);
	typeInBov     = bov_getDataFormat(bov);
	numComponents = bov_getDataComponents(bov);
	type          = local_translateBovTypeToGridType(typeInBov);

	var = dataVar_new(name, type, numComponents);

	xfree(name);

	return var;
}

static dataVarType_t
local_translateBovTypeToGridType(bovFormat_t typeInBov)
{
	dataVarType_t type;

	if (typeInBov == BOV_FORMAT_INT) {
		type = DATAVARTYPE_INT;
	} else if (typeInBov == BOV_FORMAT_BYTE) {
		type = DATAVARTYPE_INT8;
	} else if (typeInBov == BOV_FORMAT_FLOAT) {
		if (dataVarType_isNativeFloat(DATAVARTYPE_FPV)) {
			type = DATAVARTYPE_FPV;
		} else {
			fprintf(stderr, "There is no float capacity in the grid :(");
			diediedie(EXIT_FAILURE);
		}
	} else if (typeInBov == BOV_FORMAT_DOUBLE) {
		type = DATAVARTYPE_DOUBLE;
	} else {
		fprintf(stderr, "Data format of bov not supported :(\n");
		diediedie(EXIT_FAILURE);
	}

	return type;
}

static bovFormat_t
local_translateGridTypeToBovType(dataVarType_t type)
{
	bovFormat_t typeAsBovType;

	if (type == DATAVARTYPE_INT) {
		typeAsBovType = BOV_FORMAT_INT;
	} else if (type == DATAVARTYPE_INT8) {
		typeAsBovType = BOV_FORMAT_BYTE;
	} else if (type == DATAVARTYPE_DOUBLE) {
		typeAsBovType = BOV_FORMAT_DOUBLE;
	} else if (type == DATAVARTYPE_FPV) {
		if (dataVarType_isNativeFloat(type))
			typeAsBovType = BOV_FORMAT_FLOAT;
		else
			typeAsBovType = BOV_FORMAT_DOUBLE;
	} else {
		fprintf(stderr, "Grid type not compatible with bov type.");
		diediedie(EXIT_FAILURE);
	}
	return typeAsBovType;
}

static void
local_handleFilenameChange(gridReader_t reader)
{
	assert(reader != NULL);
	assert(reader->type == GRIDIO_TYPE_BOV);

	bov_t bov;

	bov = bov_newFromFile(filename_getFullName(reader->fileName));
	gridReaderBov_setBov((gridReaderBov_t)reader, bov);
}
