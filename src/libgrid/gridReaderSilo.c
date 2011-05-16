// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#ifdef WITH_SILO
#  include "gridReaderSilo.h"
#  include <assert.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <silo.h>
#  include "gridPatch.h"
#  include "../libdata/dataVar.h"
#  include "../libdata/dataVarType.h"
#  include "../libutil/xmem.h"
#  include "../libutil/parse_ini.h"
#  include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#  include "gridReader_adt.h"
#  include "gridReaderSilo_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Local variables ---------------------------------------------------*/
static struct gridReader_func_struct local_func
    = { &gridReaderSilo_del,
	    &gridReaderSilo_readIntoPatch,
	    &gridReaderSilo_readIntoPatchForVar };


/*--- Prototypes of local functions -------------------------------------*/
static int
local_translateGridTypeToSiloType(dataVarType_t type);


/*--- Implementations of exported functios ------------------------------*/
extern gridReaderSilo_t
gridReaderSilo_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReaderSilo_t reader;

	reader       = xmalloc(sizeof(struct gridReaderSilo_struct));
	reader->type = GRIDIO_TYPE_SILO;
	reader->func = (gridReader_func_t)&local_func;

	return reader;
}

extern void
gridReaderSilo_del(gridReader_t *reader)
{
	gridReaderSilo_t tmp;

	assert(reader != NULL && *reader != NULL);
	tmp = (gridReaderSilo_t)*reader;
	assert(tmp->type == GRIDIO_TYPE_SILO);

	xfree(*reader);

	*reader = NULL;
}

extern void
gridReaderSilo_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	dataVar_t var;
	int       idxOfVar;

	assert(reader->type == GRIDIO_TYPE_SILO);
	assert(patch != NULL);

#  define r ((gridReaderSilo_t)reader)
	var      = dataVar_new(r->varName, r->varType, r->varNumComponents);
#  undef r
	idxOfVar = gridPatch_attachVar(patch, var);
	dataVar_del(&var);

	gridReader_readIntoPatchForVar(reader, patch, idxOfVar);
}

extern void
gridReaderSilo_readIntoPatchForVar(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar)
{
	dataVar_t     var;
	dataVarType_t type;
	void          *data;
	int           typeAsSiloType;
	int           numComponents;
	uint32_t      dims[NDIM];
	uint32_t      idxLo[NDIM];

	assert(reader->type == GRIDIO_TYPE_SILO);
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < gridPatch_getNumVars(patch));

	var            = gridPatch_getVarHandle(patch, idxOfVar);
	data           = gridPatch_getVarDataHandle(patch, idxOfVar);

	type           = dataVar_getType(var);
	typeAsSiloType = local_translateGridTypeToSiloType(type);
	numComponents  = dataVar_getNumComponents(var);

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);

//	siloRoot_readWindowed(((gridReaderSilo_t)reader)->siloRoot, data,
//	                      typeAsSiloType, numComponents, idxLo, dims);
}

/*--- Implementations of local functions --------------------------------*/
static int
local_translateGridTypeToSiloType(dataVarType_t type)
{
	int typeAsSiloType;

	if (type == DATAVARTYPE_INT) {
		typeAsSiloType = DB_INT;
	} else if (type == DATAVARTYPE_INT8) {
		typeAsSiloType = DB_CHAR;
	} else if (type == DATAVARTYPE_DOUBLE) {
		typeAsSiloType = DB_DOUBLE;
	} else if (type == DATAVARTYPE_FPV) {
		if (dataVarType_isNativeFloat(type))
			typeAsSiloType = DB_FLOAT;
		else
			typeAsSiloType = DB_DOUBLE;
	} else {
		typeAsSiloType = DB_NOTYPE;
		fprintf(stderr, "Grid type not compatible with silo type.");
		diediedie(EXIT_FAILURE);
	}

	return typeAsSiloType;
}

#endif /* WITH_SILO */
