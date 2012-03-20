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

/** @brief  Stores the functions table for the HDF5 reader. */
static struct gridReader_func_struct local_func
    = {&gridReaderHDF5_del,
	   &gridReaderHDF5_readIntoPatch,
	   &gridReaderHDF5_readIntoPatchForVar};

/*--- Prototypes of local functions -------------------------------------*/
static void
local_handleFilenameChange(gridReader_t reader);


/*--- Implementations of exported functions -----------------------------*/
extern void
gridReaderHDF5_del(gridReader_t *reader)
{
	assert(reader != NULL && *reader != NULL);
	assert((*reader)->type = GRIDIO_TYPE_HDF5);

	gridReader_free(*reader);
	gridReaderHDF5_free((gridReaderHDF5_t)*reader);

	xfree(*reader);
	*reader = NULL;
}

extern void
gridReaderHDF5_readIntoPatch(gridReader_t reader, gridPatch_t patch)
{
	assert(reader != NULL);
	assert(reader->type = GRIDIO_TYPE_HDF5);
	assert(patch != NULL);

	/** 
	 * @bug  This function does nothing as it is not implemented.
	 * @todo Figure out the vars in the HDF5 file, create a gridVar for each
	 * and attach to patch, read the data from the file into the patch via
	 * gridReaderHDF5_readIntoPatchForVar().
	 */
#if 0
	dataVar_t var;
	int       idxOfVar;

	var      = local_getNewVar(((gridReaderHDF5_t)reader)->file);
	idxOfVar = gridPatch_attachVar(patch, var);

	gridReader_readIntoPatchForVar(reader, patch, idxOfVar);

	dataVar_del(&var);
#endif
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

/*--- Implementations of final functions --------------------------------*/
extern gridReaderHDF5_t
gridReaderHDF5_new(void)
{
	gridReaderHDF5_t reader;

	reader = gridReaderHDF5_alloc();

	gridReader_init((gridReader_t)reader, GRIDIO_TYPE_HDF5, &local_func,
	                &local_handleFilenameChange);
	gridReaderHDF5_init(reader);

	return reader;
}

extern hid_t
gridReaderHDF5_getH5File(const gridReaderHDF5_t reader)
{
	assert(reader != NULL);

	return reader->file;
}

/*--- Implementations of protected functions ----------------------------*/
extern gridReaderHDF5_t
gridReaderHDF5_alloc(void)
{
	return xmalloc(sizeof(struct gridReaderHDF5_struct));
}

extern void
gridReaderHDF5_init(gridReaderHDF5_t reader)
{
	reader->file = H5I_INVALID_HID;
}

extern void
gridReaderHDF5_free(gridReaderHDF5_t reader)
{
	if (reader->file != H5I_INVALID_HID)
		H5Fclose(reader->file);

}

extern void
gridReaderHDF5_setH5File(gridReaderHDF5_t reader, hid_t file)
{
	assert(reader != NULL);

	if (file != reader->file) {
		if (reader->file != H5I_INVALID_HID)
			H5Fclose(reader->file);
		reader->file = file;
	}
}


/*--- Implementations of local functions --------------------------------*/
extern void
local_handleFilenameChange(gridReader_t reader)
{
	assert(reader != NULL);
	assert(reader->type == GRIDIO_TYPE_HDF5);

	const char *fileName = filename_getFullName(reader->fileName);

	hid_t file = H5Fopen(fileName, H5F_ACC_RDONLY, H5P_DEFAULT);
	if (file < 0) {
		fprintf(stderr, "ERROR: Could not open %s for reading.\n", fileName);
		diediedie(EXIT_FAILURE);
	}

	gridReaderHDF5_setH5File((gridReaderHDF5_t)reader, file);
}

