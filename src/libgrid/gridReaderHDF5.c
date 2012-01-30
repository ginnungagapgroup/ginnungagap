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
#include "gridUtilHDF5.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


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
gridReaderHDF5_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReaderHDF5_t reader;
	char             *fileName;

	reader           = xmalloc(sizeof(struct gridReaderHDF5_struct));
	reader->type     = GRIDIO_TYPE_HDF5;
	reader->func     = (gridReader_func_t)&local_func;
	reader->fileName = NULL;

	getFromIni(&fileName, parse_ini_get_string,
	           ini, "fileName", sectionName);
	gridReaderHDF5_setFileName(reader, fileName);
	xfree(fileName);

	reader->file = H5Fopen(reader->fileName, H5F_ACC_RDONLY, H5P_DEFAULT);
	if (reader->file < 0) {
		fprintf(stderr, "ERROR: Could not open %s for reading.\n",
		        reader->fileName);
		diediedie(EXIT_FAILURE);
	}

	return reader;
}

extern void
gridReaderHDF5_del(gridReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);

	gridReaderHDF5_t thisReader = (gridReaderHDF5_t)(*reader);

	xfree(thisReader->fileName);
	H5Fclose(thisReader->file);
	xfree(*reader);

	*reader = NULL;
}

extern void
gridReaderHDF5_setFileName(gridReaderHDF5_t reader, const char *fileName)
{
	assert(reader != NULL);
	assert(reader->type = GRIDIO_TYPE_HDF5);
	assert(fileName != NULL);

	if (reader->fileName != NULL)
		xfree(reader->fileName);

	reader->fileName = xstrdup(fileName);
}

extern void
gridReaderHDF5_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	dataVar_t var;
	int       idxOfVar;

	assert(reader != NULL);
	assert(reader->type = GRIDIO_TYPE_HDF5);
	assert(patch != NULL);

	//var      = local_getNewVar(((gridReaderHDF5_t)reader)->file);
	idxOfVar = gridPatch_attachVar(patch, var);

	gridReader_readIntoPatchForVar(reader, patch, idxOfVar);

	dataVar_del(&var);
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

	hid_t             dataSet;
	hid_t             dataSpaceFile, dataTypeFile;
	hid_t             dataSpacePatch, dataTypePatch;
	gridPointUint32_t idxLoPatch, dimsPatch;
	dataVar_t         var   = gridPatch_getVarHandle(patch, idxOfVar);
	void              *data = gridPatch_getVarDataHandle(patch, idxOfVar);

	gridPatch_getIdxLo(patch, idxLoPatch);
	gridPatch_getDims(patch, dimsPatch);

	dataSet        = H5Dopen(((gridReaderHDF5_t)reader)->file,
	                         dataVar_getName(var), H5P_DEFAULT);
	dataTypeFile   = H5Dget_type(dataSet);
	dataSpaceFile  = H5Dget_space(dataSet);

	dataTypePatch  = dataVar_getHDF5Datatype(var);
	dataSpacePatch = gridUtilHDF5_getDataSpaceFromDims(dimsPatch);

	gridUtilHDF5_selectHyperslab(dataSpaceFile, idxLoPatch, dimsPatch);

	if (H5Tequal(dataTypeFile, dataTypePatch)) {
		H5Dread(dataSet, dataTypeFile, dataSpacePatch,
		        dataSpaceFile, H5P_DEFAULT, data);
	} else {
		fprintf(stderr, "ERROR: Datatype in memory differs from file.\n");
		diediedie(EXIT_FAILURE);
	}

	H5Sclose(dataSpacePatch);
	H5Tclose(dataTypePatch);
	H5Sclose(dataSpaceFile);
	H5Tclose(dataTypeFile);
	H5Dclose(dataSet);
} /* gridReaderHDF5_readIntoPatchForVar */

/*--- Implementations of local functions --------------------------------*/
