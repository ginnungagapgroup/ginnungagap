// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderBov.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "gridPatch.h"
#include "gridVar.h"
#include "../libutil/xmem.h"
#include "../libutil/parse_ini.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridReader_adt.h"
#include "gridReaderBov_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct gridReader_func_struct local_func
    = {&gridReaderBov_del,
	   &gridReaderBov_readIntoPatch,
	   &gridReaderBov_readIntoPatchForVar};

/*--- Prototypes of local functions -------------------------------------*/
static gridVar_t
local_getNewVar(bov_t bov);

static gridVarType_t
local_translateBovTypeToGridType(bovFormat_t typeInBov);

static bovFormat_t
local_translateGridTypeToBovType(gridVarType_t type);


/*--- Implementations of exported functios ------------------------------*/
extern gridReaderBov_t
gridReaderBov_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReaderBov_t reader;
	char            *bovFileName;

	reader       = xmalloc(sizeof(struct gridReaderBov_struct));
	reader->type = IO_TYPE_BOV;
	reader->func = (gridReader_func_t)&local_func;

	getFromIni(&bovFileName, parse_ini_get_string,
	           ini, "bovFileName", sectionName);
	reader->bov = bov_newFromFile(bovFileName);
	xfree(bovFileName);

	return reader;
}

extern void
gridReaderBov_del(gridReader_t *reader)
{
	gridReaderBov_t tmp;

	assert(reader != NULL && *reader != NULL);
	assert(*reader->type == IO_TYPE_BOV);

	tmp = (gridReaderBov_t)*reader;
	bov_del(&(tmp->bov));

	xfree(*reader);

	*reader = NULL;
}

extern void
gridReader_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	gridReaderBov_t readerBov;
	gridVar_t       var;
	int             idxOfVar;

	assert(reader->type == IO_TYPE_BOV);
	assert(patch != NULL);

	var      = local_getNewVar(reader->bov);
	idxOfVar = gridPatch_attachVarData(patch, var);

	gridReader_redIntoPatchForVar(reader, patch, idxOfVar);

	gridVar_del(&var);
}

extern void
gridReader_readIntoPatchForVar(gridReader_t reader,
                               gridPatch_t  patch,
                               int          idxOfVar)
{
	gridReaderBov_t readerBov;
	gridVar_t       var;
	gridVarType_t   type;
	void            *data;
	bovFormat_t     typeAsBovType;
	int             numComponents;

	assert(reader->type == IO_TYPE_BOV);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));

	readerBov     = (gridReaderBov_t)reader;
	var           = gridPatch_getVarHandle(patch, idxOfVar);
	data          = gridPatch_getVarDataHandle(patch, idxOfVar);

	type          = gridVar_getType(var);
	typeAsBovType = local_translateGridTypeToBovType(type);
	numComponents = gridVar_getNumComponents(var);

//	bov_readInto
}

/*--- Implementations of local functions --------------------------------*/
static gridVar_t
local_getNewVar(bov_t bov)
{
	char          *name;
	gridVarType_t type;
	bovFormat_t   typeInBov;
	int           numComponents;

	name          = boc_getVarName(bov);
	typeInBov     = bov_getDataFormat(bov);
	numComponents = bov_getDataComponents(bov);

	type          = local_translateBovTypeToGridType(typeInBov);

	return gridVar_new(name, type, numComponents);
}

static gridVarType_t
local_translateBovTypeToGridType(bovFormat_t typeInBov)
{
	gridVarType_t type;

	if (typeInBov == BOV_FORMAT_INT) {
		type = GRIDVARTYPE_INT;
	} else if (typeInBov == BOV_FORMAT_FLOAT) {
		if (gridVarType_isNativeFloat(GRIDVARTYPE_FPV)) {
			type = GRIDVARTYPE_FPV;
		} else {
			fprintf(stderr, "There is no float capacity in the grid :(");
			diediedie(EXIT_FAILURE);
		}
	} else if (typeInBov == BOV_FORMAT_DOUBLE) {
		type = GRIDVARTYPE_DOUBLE;
	} else {
		fprintf(stderr, "Data format of bov not supported :(\n");
		diediedie(EXIT_FAILURE);
	}

	return type;
}

static bovFormat_t
local_translateGridTypeToBovType(gridVarType_t type)
{
	bovFormat_t typeAsBovType;

	if (type == GRIDVARTYPE_INT) {
		typeAsBovType = BOV_FORMAT_INT;
	} else if (type == GRIDVARTYPE_DOUBLE) {
		typeAsBovType = BOV_FORMAT_DOUBLE;
	} else if (type == GRIDVARTYPE_FPV) {
		if (gridVarType_isNativeFloat(type))
			typeAsBovType == BOV_FORMAT_FLOAT;
		else
			typeAsBovType == BOV_FORMAT_DOUBLE;
	} else {
		fprintf(stderr, "Grid type not compatible with bov type.");
		diediedie(EXIT_FAILURE);
	}
	return typeAsBovType;
}
