// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderGrafic.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "gridPatch.h"
#include "gridVar.h"
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
static struct gridReader_func_struct local_func
    = { &gridReaderGrafic_del,
	    &gridReaderGrafic_readIntoPatch,
	    &gridReaderGrafic_readIntoPatchForVar };

/*--- Prototypes of local functions -------------------------------------*/
static gridVar_t
local_getNewVar(gridReaderGrafic_t grafic);

static graficFormat_t
local_translateGridTypeToGraficType(gridVarType_t type);


/*--- Implementations of exported functios ------------------------------*/
extern gridReaderGrafic_t
gridReaderGrafic_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReaderGrafic_t reader;
	char               *graficFileName;
	bool               isWhiteNoise;

	reader       = xmalloc(sizeof(struct gridReaderGrafic_struct));
	reader->type = IO_TYPE_GRAFIC;
	reader->func = (gridReader_func_t)&local_func;

	getFromIni(&graficFileName, parse_ini_get_string,
	           ini, "graficFileName", sectionName);
	getFromIni(&isWhiteNoise, parse_ini_get_bool,
	           ini, "isWhiteNoise", sectionName);

	reader->grafic = grafic_newFromFile(graficFileName,
	                                    isWhiteNoise);

	xfree(graficFileName);

	return reader;
}

extern void
gridReaderGrafic_del(gridReader_t *reader)
{
	gridReaderGrafic_t tmp;

	assert(reader != NULL && *reader != NULL);
	tmp = (gridReaderGrafic_t)*reader;
	assert(tmp->type == IO_TYPE_GRAFIC);

	grafic_del(&(tmp->grafic));
	xfree(*reader);

	*reader = NULL;
}

extern void
gridReaderGrafic_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	gridVar_t var;
	int       idxOfVar;

	assert(reader->type == IO_TYPE_GRAFIC);
	assert(patch != NULL);

	var      = local_getNewVar((gridReaderGrafic_t)reader);
	idxOfVar = gridPatch_attachVarData(patch, var);

	gridReader_readIntoPatchForVar(reader, patch, idxOfVar);

	gridVar_del(&var);
}

extern void
gridReaderGrafic_readIntoPatchForVar(gridReader_t reader,
                                     gridPatch_t  patch,
                                     int          idxOfVar)
{
	gridVar_t      var;
	gridVarType_t  type;
	void           *data;
	graficFormat_t typeAsGraficType;
	int            numComponents;
	uint32_t       dims[3];
	uint32_t       idxLo[3];

	assert(reader->type == IO_TYPE_GRAFIC);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));

	var              = gridPatch_getVarHandle(patch, idxOfVar);
	data             = gridPatch_getVarDataHandle(patch, idxOfVar);

	type             = gridVar_getType(var);
	typeAsGraficType = local_translateGridTypeToGraficType(type);
	numComponents    = gridVar_getNumComponents(var);

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);
#if (NDIM == 2)
	idxLo[2] = 0;
	dims[2]  = 1;
#endif

	grafic_readWindowed(((gridReaderGrafic_t)reader)->grafic, data,
	                    typeAsGraficType, numComponents, idxLo, dims);
}

/*--- Implementations of local functions --------------------------------*/
static gridVar_t
local_getNewVar(gridReaderGrafic_t reader)
{
	char      *name;
	gridVar_t var;

	name = xbasename(grafic_getFileName(reader->grafic));
	var  = gridVar_new(name, GRIDVARTYPE_FPV, 1);

	xfree(name);

	return var;
}

static graficFormat_t
local_translateGridTypeToGraficType(gridVarType_t type)
{
	graficFormat_t typeAsGraficType;

	if (type == GRIDVARTYPE_DOUBLE) {
		typeAsGraficType = GRAFIC_FORMAT_DOUBLE;
	} else if (type == GRIDVARTYPE_FPV) {
		if (gridVarType_isNativeFloat(type))
			typeAsGraficType = GRAFIC_FORMAT_FLOAT;
		else
			typeAsGraficType = GRAFIC_FORMAT_DOUBLE;
	} else {
		fprintf(stderr, "Grid type not compatible with grafic type.");
		diediedie(EXIT_FAILURE);
	}
	return typeAsGraficType;
}
