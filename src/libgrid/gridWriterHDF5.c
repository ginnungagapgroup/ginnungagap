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
#include <string.h>
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/diediedie.h"


/*--- Implementation of main structure ----------------------------------*/
#include "gridWriterHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Creates a new HDF5 file.
 *
 * @param[in]  writer
 *                The writer object containing the necessary information
 *                to create the HDF5 file.
 *
 * @return  Returns a handle to the newly created file.
 */
static hid_t
local_createFile(const gridWriterHDF5_t writer);


/**
 * @brief  Creates an HDF5 data space for a given size.
 *
 * @param[in]  dims
 *                The size of the data space.
 *
 * @return  Returns a proper HDF5 data space for the given size.
 */
static hid_t
local_getDataSpaceFromDims(gridPointUint32_t dims);

static hid_t
local_getDSCreationPropList(gridWriterHDF5_t writer);

inline static void
local_writeVariableAtPatch(dataVar_t   var,
                           gridPatch_t patch,
                           hid_t       dt,
                           hid_t       gridSize,
                           hid_t       fileHandle,
                           hid_t       dsCreationPropList);

inline static void
local_selectHyperslab(hid_t             ds,
                      gridPointUint32_t idxLo,
                      gridPointUint32_t dims);

static bool
local_checkIfPatchIsCompleteGrid(hid_t space, gridPointUint32_t patchDims);


/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the functions table for the HDF5 writer. */
static struct gridWriter_func_struct local_func
    = { &gridWriterHDF5_del,
	    &gridWriterHDF5_activate,
	    &gridWriterHDF5_deactivate,
	    &gridWriterHDF5_writeGridPatch,
	    &gridWriterHDF5_writeGridRegular,
#ifdef WITH_MPI
	    &gridWriterHDF5_initParallel
#endif
	};

/** @brief  Gives the default name of the output file. */
static const char *local_defaultFileName = "outGrid.h5";


/*--- Implementations of exported functions -----------------------------*/
extern gridWriterHDF5_t
gridWriterHDF5_new(void)
{
	gridWriterHDF5_t writer;

	writer             = xmalloc(sizeof(struct gridWriterHDF5_struct));
	writer->type       = GRIDIO_TYPE_HDF5;
	writer->func       = (gridWriter_func_t)&local_func;

	writer->isActive   = false;
	writer->fileHandle = H5I_INVALID_HID;
	writer->fileName   = local_defaultFileName;
#ifdef WITH_MPI
	writer->mpiComm    = MPI_COMM_NULL;
#endif
	writer->doChunking = false;
	for (int i = 0; i < NDIM; i++)
		writer->chunkSize[i] = 0;
	writer->doChecksum        = false;
	writer->doCompression     = false;
	writer->compressionFilter = H5I_INVALID_HID;

	return writer;
}

extern gridWriterHDF5_t
gridWriterHDF5_newFromIni(parse_ini_t ini, const char *sectionName)
{
	assert(ini != NULL);
	assert(sectionName != NULL);

	gridWriterHDF5_t writer;
	char             *fileName;
	bool             tmp, doChunking, doChecksum, doCompression;


	writer = gridWriterHDF5_new();
	// Required
	getFromIni(&fileName, parse_ini_get_string, ini,
	           "fileName", sectionName);
	gridWriterHDF5_setFileName(writer, fileName);
	xfree(fileName);
	// Optional
	tmp = parse_ini_get_bool(ini, "doChunking", sectionName, &doChunking);
	if (tmp && doChunking) {
		int32_t           *sizeFile;
		gridPointUint32_t sizeCode;
		if (!parse_ini_get_int32list(ini, "chunkSize", sectionName,
		                             NDIM, (int32_t **)&sizeFile)) {
			fprintf(stderr, "Could not get chunkSize from section %s.\n",
			        sectionName);
			diediedie(EXIT_FAILURE);
		}
		for (int i = 0; i < NDIM; i++)
			sizeCode[i] = sizeFile[i];
		xfree(sizeFile);
		gridWriterHDF5_setChunkSize(writer, sizeCode);
	}

	if (parse_ini_get_bool(ini, "doChecksum", sectionName, &doChecksum))
		gridWriterHDF5_setDoChecksum(writer, doChecksum);

	tmp = parse_ini_get_bool(ini, "doCompression", sectionName,
	                         &doCompression);
	if (tmp && doCompression) {
		char *filterName;
		getFromIni(&filterName, parse_ini_get_string, ini,
		           "filterName", sectionName);
	}


	return writer;
} /* gridWriterHDF5_newFromIni */

extern void
gridWriterHDF5_del(gridWriter_t *writer)
{
	assert(writer != NULL && *writer != NULL);
	gridWriterHDF5_t tmp = (gridWriterHDF5_t)*writer;
	assert(tmp->type == GRIDIO_TYPE_HDF5);

	if (tmp->isActive)
		gridWriter_deactivate(*writer);
	assert(tmp->fileHandle == H5I_INVALID_HID);
	if (tmp->fileName != local_defaultFileName)
		xfree((char *)tmp->fileName);
	xfree(*writer);

	*writer = NULL;
}

extern void
gridWriterHDF5_setFileName(gridWriterHDF5_t w, const char *fileName)
{
	assert(w != NULL);
	assert(fileName != NULL);

	if (w->fileName != local_defaultFileName)
		xfree((char *)w->fileName);
	w->fileName = xstrdup(fileName);
}

extern void
gridWriterHDF5_setDoChunking(gridWriterHDF5_t w, bool doChunking)
{
	assert(w != NULL);

	w->doChunking = doChunking;
}

extern void
gridWriterHDF5_setChunkSize(gridWriterHDF5_t w, gridPointUint32_t chunkSize)
{
	assert(w != NULL);

	if (!w->doChunking)
		w->doChunking = true;

	for (int i = 0; i < NDIM; i++)
		w->chunkSize[i] = (hsize_t)(chunkSize[NDIM - 1 - i]);
}

extern void
gridWriterHDF5_setDoChecksum(gridWriterHDF5_t w, bool doChecksum)
{
	assert(w != NULL);

	w->doChecksum = doChecksum;
	if (w->doChecksum)
		assert(H5Zfilter_avail(H5Z_FILTER_FLETCHER32));
}

extern void
gridWriterHDF5_setDoCompression(gridWriterHDF5_t w, bool doCompression)
{
	assert(w != NULL);

	if (w->doCompression && !doCompression)
		w->compressionFilter = H5I_INVALID_HID;

	w->doCompression = doCompression;

	if (w->doCompression && (w->compressionFilter == H5I_INVALID_HID))
		w->compressionFilter = H5Z_FILTER_DEFLATE;
}

extern void
gridWriterHDF5_setCompressionFilter(gridWriterHDF5_t w,
                                    const char       *filterName)
{
	assert(w != NULL);

	if (!w->doCompression)
		w->doCompression = true;

	if (strcmp(filterName, "gzip") == 0) {
		assert(H5Zfilter_avail(H5Z_FILTER_DEFLATE));
		w->compressionFilter = H5Z_FILTER_DEFLATE;
	} else if (strcmp(filterName, "szip") == 0) {
		assert(H5Zfilter_avail(H5Z_FILTER_SZIP));
		w->compressionFilter = H5Z_FILTER_SZIP;
	} else {
		fprintf(stderr, "Unknown compression filter %s.\n", filterName);
		w->compressionFilter = H5I_INVALID_HID;
		w->doCompression     = false;
	}
}

extern void
gridWriterHDF5_activate(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->type == GRIDIO_TYPE_HDF5);

	gridWriterHDF5_t tmp = (gridWriterHDF5_t)writer;

	if (!tmp->isActive) {
		assert(tmp->fileHandle == H5I_INVALID_HID);
		tmp->fileHandle = local_createFile(tmp);
		tmp->isActive   = true;
	}
}

extern void
gridWriterHDF5_deactivate(gridWriter_t writer)
{
	assert(writer != NULL);
	assert(writer->type == GRIDIO_TYPE_HDF5);

	gridWriterHDF5_t tmp = (gridWriterHDF5_t)writer;

	if (tmp->isActive) {
		H5Fclose(tmp->fileHandle);
		tmp->fileHandle = H5I_INVALID_HID;
		tmp->isActive   = false;
	}
}

extern void
gridWriterHDF5_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta)
{
	assert(writer != NULL);
	assert(writer->type == GRIDIO_TYPE_HDF5);
	assert(patch != NULL);
	assert(patchName != NULL);
	assert(writer->isActive);

	gridWriterHDF5_t  tmp     = (gridWriterHDF5_t)writer;
	int               numVars = gridPatch_getNumVars(patch);
	gridPointUint32_t dims;
	hid_t             patchSize, dsCreationPropList;

	gridPatch_getDims(patch, dims);
	patchSize          = local_getDataSpaceFromDims(dims);
	dsCreationPropList = local_getDSCreationPropList(tmp);

	for (int i = 0; i < numVars; i++) {
		dataVar_t var = gridPatch_getVarHandle(patch, i);
		hid_t     dt  = dataVar_getHDF5Datatype(var);

		local_writeVariableAtPatch(var, patch, dt, patchSize,
		                           tmp->fileHandle, dsCreationPropList);
	}
}

extern void
gridWriterHDF5_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid)
{
	assert(writer != NULL);
	assert(writer->type == GRIDIO_TYPE_HDF5);
	assert(grid != NULL);
	assert(writer->isActive);

	gridWriterHDF5_t  tmp = (gridWriterHDF5_t)writer;
	int               numVars, numPatches;
	gridPointUint32_t dims;
	hid_t             gridSize, dsCreationPropList;

	numVars    = gridRegular_getNumVars(grid);
	numPatches = gridRegular_getNumPatches(grid);

	gridRegular_getDims(grid, dims);
	gridSize           = local_getDataSpaceFromDims(dims);
	dsCreationPropList = local_getDSCreationPropList(tmp);

	for (int i = 0; i < numVars; i++) {
		dataVar_t var = gridRegular_getVarHandle(grid, i);
		hid_t     dt  = dataVar_getHDF5Datatype(var);
		for (int j = 0; j < numPatches; j++) {
			gridPatch_t patch = gridRegular_getPatchHandle(grid, j);
			assert(tmp->fileHandle != H5I_INVALID_HID);
			local_writeVariableAtPatch(var, patch, dt, gridSize,
			                           tmp->fileHandle, dsCreationPropList);
		}
	}
	H5Sclose(gridSize);
}

#ifdef WITH_MPI
extern void
gridWriterHDF5_initParallel(gridWriter_t writer, MPI_Comm mpiComm)
{
	gridWriterHDF5_t tmp = (gridWriterHDF5_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_HDF5);

	tmp->mpiComm = mpiComm;
}

#endif


/*--- Implementations of local functions --------------------------------*/
static hid_t
local_createFile(const gridWriterHDF5_t writer)
{
	hid_t fileHandle;
	hid_t accessProp = H5P_DEFAULT;
#ifdef WITH_MPI
	int   rtn;

	accessProp = H5Pcreate(H5P_FILE_ACCESS);
	if (accessProp < 0)
		diediedie(EXIT_FAILURE);
	rtn = H5Pset_fapl_mpio(accessProp, writer->mpiComm, MPI_INFO_NULL);
	if (rtn < 0)
		diediedie(EXIT_FAILURE);
#endif

	fileHandle = H5Fcreate(writer->fileName, H5F_ACC_EXCL,
	                       H5P_DEFAULT, accessProp);
	assert(fileHandle >= 0);
	if (accessProp != H5P_DEFAULT)
		H5Pclose(accessProp);

	return fileHandle;
}

static hid_t
local_getDataSpaceFromDims(gridPointUint32_t dims)
{
	hid_t   ds;
	hsize_t dsDims[NDIM];

	for (int i = 0; i < NDIM; i++)
		dsDims[i] = dims[NDIM - 1 - i];

	ds = H5Screate_simple(NDIM, dsDims, NULL);
	if (ds < 0)
		diediedie(EXIT_FAILURE);

	return ds;
}

static hid_t
local_getDSCreationPropList(gridWriterHDF5_t writer)
{
	hid_t rtn = H5P_DEFAULT;

	if (writer->doChunking) {
		herr_t err;
		rtn = H5Pcreate(H5P_DATASET_CREATE);
		assert(rtn >= 0);

		err = H5Pset_chunk(rtn, NDIM, writer->chunkSize);
		if (err < 0)
			diediedie(EXIT_FAILURE);

		if (writer->doChecksum) {
			err = H5Pset_filter(rtn, H5Z_FILTER_FLETCHER32,
			                    H5Z_FLAG_MANDATORY, 0, NULL);
		if (err < 0)
			diediedie(EXIT_FAILURE);
		}
		if (writer->doCompression) {
			err = H5Pset_filter(rtn, writer->compressionFilter,
			                    H5Z_FLAG_OPTIONAL, 0, NULL);
		if (err < 0)
			diediedie(EXIT_FAILURE);
		}
	}

	return rtn;
}

inline static void
local_writeVariableAtPatch(dataVar_t   var,
                           gridPatch_t patch,
                           hid_t       dt,
                           hid_t       gridSize,
                           hid_t       fileHandle,
                           hid_t       dsCreationPropList)
{
	hid_t             dataSet;
	hid_t             transProps = H5P_DEFAULT;
	gridPointUint32_t dimsPatch;
	hid_t             dataSpacePatch, dataSpaceFile;
	void              *data = gridPatch_getVarDataHandleByVar(patch, var);

	dataSet = H5Dcreate(fileHandle, dataVar_getName(var),
	                    dt, gridSize,
	                    H5P_DEFAULT,
	                    dsCreationPropList,
	                    H5P_DEFAULT);
#ifdef WITH_MPI
	transProps = H5Pcreate(H5P_DATASET_XFER);
	assert(transProps >= 0);
	H5Pset_dxpl_mpio(transProps, H5FD_MPIO_COLLECTIVE);
#endif

	dataSpaceFile = H5Dget_space(dataSet);
	assert(dataSpaceFile >= 0);
	gridPatch_getDims(patch, dimsPatch);
	if (local_checkIfPatchIsCompleteGrid(gridSize, dimsPatch))
		local_selectHyperslab(dataSpaceFile, NULL, dimsPatch);
	else {
		gridPointUint32_t idxLo;
		gridPatch_getIdxLo(patch, idxLo);
		local_selectHyperslab(dataSpaceFile, idxLo, dimsPatch);
	}
	dataSpacePatch = local_getDataSpaceFromDims(dimsPatch);

	H5Dwrite(dataSet, dt, dataSpacePatch, dataSpaceFile,
	         transProps, data);

	H5Sclose(dataSpacePatch);
	H5Sclose(dataSpaceFile);
	if (transProps != H5P_DEFAULT)
		H5Pclose(transProps);
	H5Dclose(dataSet);
} /* local_writeVariableAtPatch */

inline static void
local_selectHyperslab(hid_t             ds,
                      gridPointUint32_t idxLo,
                      gridPointUint32_t dims)
{
	hsize_t dsDims[NDIM];
	hsize_t dsIdxLo[NDIM];

	for (int i = 0; i < NDIM; i++) {
		dsDims[i]  = dims[NDIM - 1 - i];
		dsIdxLo[i] = (idxLo == NULL) ? 0 : idxLo[NDIM - 1 - i];
	}

	H5Sselect_hyperslab(ds, H5S_SELECT_SET, dsIdxLo, NULL, dsDims, NULL);
}

static bool
local_checkIfPatchIsCompleteGrid(hid_t space, gridPointUint32_t patchDims)
{
	bool    patchIsCompleteGrid = true;
	hsize_t spaceDims[NDIM];

	H5Sget_simple_extent_dims(space, spaceDims, NULL);
	for (int i = 0; i < NDIM; i++) {
		assert(spaceDims[i] >= (hsize_t)(patchDims[NDIM - 1 - i]));
		if (spaceDims[i] != (hsize_t)(patchDims[NDIM - 1 - i]))
			patchIsCompleteGrid = false;
	}

	return patchIsCompleteGrid ? true : false;
}
