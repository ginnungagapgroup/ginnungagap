// Copyright (C) 2011, 2012, Steffen Knollmann
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
#include <string.h>
#include <hdf5.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "gridUtilHDF5.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/diediedie.h"


/*--- Implementation of main structure ----------------------------------*/
#include "gridWriterHDF5_adt.h"


/*--- Local defines -----------------------------------------------------*/
 #define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
 #define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the functions table for the HDF5 writer. */
static struct gridWriter_func_struct local_func
    = {&gridWriterHDF5_del,
	   &gridWriterHDF5_activate,
	   &gridWriterHDF5_deactivate,
	   &gridWriterHDF5_writeGridPatch,
	   &gridWriterHDF5_writeGridRegular,
#ifdef WITH_MPI
	   &gridWriterHDF5_initParallel
#endif
	};

/** @brief  Gives the default path of the output file. */
static const char *local_defaultFileNamePath = NULL;

/** @brief  Gives the default prefix of the output file. */
static const char *local_defaultFileNamePrefix = "out";

/** @brief  Gives the default qualifier of the output file. */
static const char *local_defaultFileNameQualifier = NULL;

/** @brief  Gives the default suffix of the output file. */
static const char *local_defaultFileNameSuffix = ".h5";


/*--- Prototypes of local functions -------------------------------------*/

static void
local_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid);
static void
local_writeGridRtw(gridWriter_t  writer,
                                gridRegular_t grid);

/**
 * @brief  Get the HDF5 file handle for the required file.
 *
 * @param[in]  writer
 *                The writer object containing the necessary information
 *                to create the HDF5 file.
 *
 * @return  Returns a handle to the newly created or opened file.
 */
static hid_t
local_getFileHandle(const gridWriterHDF5_t writer);


#ifdef WITH_MPI

/**
 * @brief  Small helper function to get the access property list for the
 *         MPI things.
 *
 * @param[in]  comm
 *                The MPI communicator that should be used.
 *
 * @return  Returns a handle for a new access property list.
 */
static hid_t
local_getAccessPropsFileAccessMPI(MPI_Comm comm);

#endif

/**
 * @brief  Creates the property list for creating data spaces with chunking
 *         et al.
 *
 * @param[in]  writer
 *                The writer holding the information about the chunking,
 *                checksumming and compression.
 *
 * @return  Returns a new property list for use with H5Dcreate to deal with
 *          chunking, checksumming and compression.
 */
static hid_t
local_getDSCreationPropList(const gridWriterHDF5_t writer);

/**
 * @brief  Helper function to write the data of a variable at a given patch.
 *
 * @param[in]  var
 *                The variable that should be written.
 * @param[in]  patch
 *                The patch that should be written.
 * @param[in]  dataSet
 *                The HDF5 dataset to work with.
 * @param[in]  dt
 *                The HDF5 datatype corresponding to the variable.
 * @param[in]  gridSize
 *                The extent of the grid that should be written.  Note that
 *                the patch may be smaller.
 *
 * @return  Returns nothing.
 */
inline static void
local_writeVariableAtPatch(dataVar_t   var,
                           gridPatch_t patch,
                           hid_t       dataSet,
                           hid_t       dt,
                           hid_t       gridSize);


/**
 * @brief  Checks if a patch fills the whole grid.
 *
 * @param[in]  space
 *                The HDF5 space describing the whole grid.
 * @param[in]  patchDims
 *                The extent of the patch to check.
 *
 * @return  Returns @c true if the patch is the complete grid and @c false
 *          if the space is larger than the patch.
 */
static bool
local_checkIfPatchIsCompleteGrid(hid_t space, gridPointUint32_t patchDims);

inline static void
local_writeNull();


/*--- Implementations of abstract functions -----------------------------*/
extern void
gridWriterHDF5_del(gridWriter_t *writer)
{
	assert(writer != NULL && *writer != NULL);
	assert((*writer)->type == GRIDIO_TYPE_HDF5);

	gridWriter_free(*writer);
	gridWriterHDF5_free((gridWriterHDF5_t)*writer);

	xfree(*writer);
	*writer = NULL;
}

extern void
gridWriterHDF5_activate(gridWriter_t writer)
{
	gridWriterHDF5_t w = (gridWriterHDF5_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_HDF5);

	if (!gridWriter_isActive(writer)) {
		assert(w->fileHandle == H5I_INVALID_HID);
		w->fileHandle = local_getFileHandle(w);
		gridWriter_setIsActive(writer);
	}
}

extern void
gridWriterHDF5_deactivate(gridWriter_t writer)
{
	gridWriterHDF5_t w = (gridWriterHDF5_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_HDF5);

	if (gridWriter_isActive(writer)) {
		H5Fclose(w->fileHandle);
		w->fileHandle = H5I_INVALID_HID;
		gridWriter_setIsInactive(writer);
	}
}

extern void
gridWriterHDF5_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta)
{
	gridWriterHDF5_t w = (gridWriterHDF5_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_HDF5);
	assert(patch != NULL);
	assert(patchName != NULL);
	assert(w->base.isActive);

	int               numVars = gridPatch_getNumVars(patch);
	gridPointUint32_t dims;
	hid_t             patchSize, dsCreationPropList;

	gridPatch_getDims(patch, dims);
	patchSize          = gridUtilHDF5_getDataSpaceFromDims(dims);
	dsCreationPropList = local_getDSCreationPropList(w);

	for (int i = 0; i < numVars; i++) {
		dataVar_t var = gridPatch_getVarHandle(patch, i);
		hid_t     dt  = dataVar_getHDF5Datatype(var);
		hid_t     dataSet = H5Dcreate(w->fileHandle, dataVar_getName(var),
		                              dt, patchSize, H5P_DEFAULT,
		                              dsCreationPropList,
		                              H5P_DEFAULT);
		local_writeVariableAtPatch(var, patch, dataSet, dt, patchSize);
		H5Dclose(dataSet);
	}
}

extern void
gridWriterHDF5_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid)
{
	gridWriterHDF5_t w = (gridWriterHDF5_t)writer;
	assert(w != NULL);
	if(w->doPatch)
		local_writeGridRtw(writer, grid);
	else
		local_writeGridRegular(writer, grid);
}

#ifdef WITH_MPI
extern void
gridWriterHDF5_initParallel(gridWriter_t writer, MPI_Comm mpiComm)
{
	gridWriterHDF5_t w = (gridWriterHDF5_t)writer;

	assert(w != NULL);
	assert(w->type == GRIDIO_TYPE_HDF5);

	w->mpiComm = mpiComm;
}

#endif


/*--- Implementations of final functions --------------------------------*/
extern gridWriterHDF5_t
gridWriterHDF5_new(void)
{
	gridWriterHDF5_t writer;

	writer = gridWriterHDF5_alloc();

	gridWriter_init((gridWriter_t)writer, GRIDIO_TYPE_HDF5, &local_func);
	gridWriterHDF5_init(writer);

	return writer;
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

#ifdef WITH_MPI
	// No filters in parallel HDF5 writing.
	doChecksum = false;
#endif

	w->doChecksum = doChecksum;
	if (w->doChecksum)
		assert(H5Zfilter_avail(H5Z_FILTER_FLETCHER32));
}

extern void
gridWriterHDF5_setDoCompression(gridWriterHDF5_t w, bool doCompression)
{
	assert(w != NULL);

#ifdef WITH_MPI
	// No compression in parallel HDF5 writing.
	doCompression = false;
#endif

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

#ifdef WITH_MPI
	// No compression in parallel HDF5 writing.
	return;
#endif

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
gridWriterHDF5_setDoPatch(gridWriterHDF5_t w, bool doPatch)
{
	assert(w != NULL);
	w->doPatch = doPatch;
}

extern void
gridWriterHDF5_setRtw(gridWriterHDF5_t w, gridPointUint32_t Lo, gridPointUint32_t d)
{
	assert(w != NULL);
	for(int i=0;i<NDIM; i++) {
		w->rtwLo[i] = Lo[i];
		w->rtwDims[i] = d[i];
	}
}


/*--- Implementations of protected functions ----------------------------*/
extern gridWriterHDF5_t
gridWriterHDF5_alloc(void)
{
	gridWriterHDF5_t writer;

	writer = xmalloc(sizeof(struct gridWriterHDF5_struct));

	return writer;
}

extern void
gridWriterHDF5_init(gridWriterHDF5_t writer)
{
	gridWriter_setFileName((gridWriter_t)writer,
	                       filename_newFull(local_defaultFileNamePath,
	                                        local_defaultFileNamePrefix,
	                                        local_defaultFileNameQualifier,
	                                        local_defaultFileNameSuffix));

	writer->fileHandle = H5I_INVALID_HID;
#ifdef WITH_MPI
	writer->mpiComm    = MPI_COMM_NULL;
#endif
	writer->doChunking = false;
	for (int i = 0; i < NDIM; i++) {
		writer->chunkSize[i] = 0;
		writer->rtwLo[i]=0;
		writer->rtwDims[i]=0;
	}
	writer->doChecksum        = false;
	writer->doCompression     = false;
	writer->compressionFilter = H5I_INVALID_HID;
	writer->doPatch			  = false;
}

extern void
gridWriterHDF5_free(gridWriterHDF5_t writer)
{
	assert(writer->fileHandle == H5I_INVALID_HID);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_writeGridRtw(gridWriter_t  writer,
                                gridRegular_t grid)
{
	gridWriterHDF5_t w = (gridWriterHDF5_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_HDF5);
	assert(grid != NULL);
	assert(w->base.isActive);

	int               numVars, numPatches;
	gridPointUint32_t dims;
	hid_t             gridSize, dsCreationPropList;
	
	gridPointUint32_t rtwHi, idxLo, idxHi, dimsPatch;
	uint32_t	oldLo;
	bool		doWrite, doWriteGlob;
	for(int k = 0; k<NDIM; k++) 
		rtwHi[k] = w->rtwLo[k]+w->rtwDims[k]-1;

	numVars    = gridRegular_getNumVars(grid);
	numPatches = gridRegular_getNumPatches(grid);

	//gridRegular_getDims(grid, dims);
	gridSize           = gridUtilHDF5_getDataSpaceFromDims(w->rtwDims);
	dsCreationPropList = local_getDSCreationPropList(w);
	for (int i = 0; i < numVars; i++) {
		dataVar_t var     = gridRegular_getVarHandle(grid, i);
		hid_t     dt      = dataVar_getHDF5Datatype(var);
		hid_t     dataSet = H5Dcreate(w->fileHandle, dataVar_getName(var),
	                              dt, gridSize, H5P_DEFAULT,
	                              dsCreationPropList,
	                              H5P_DEFAULT);
		for (int j = 0; j < numPatches; j++) {
			gridPatch_t patch = gridRegular_getPatchHandle(grid, j);
			assert(w->fileHandle != H5I_INVALID_HID);
			
			gridPatch_getDims(patch, dimsPatch);
			gridPatch_getIdxLo(patch, idxLo);
			doWrite = true;
			
			for(int k=0; k<NDIM; k++) {
				oldLo = idxLo[k];
				idxLo[k] = MAX(idxLo[k], w->rtwLo[k]);
				idxHi[k] = MIN(oldLo+dimsPatch[k]-1, rtwHi[k]);
				if(idxLo[k]>idxHi[k]) doWrite = false;
			}
			
			hid_t             transProps = H5P_DEFAULT;
			gridPointUint32_t dimsPatch = {0,0,0};
			hid_t             dataSpacePatch, dataSpaceFile;
			
			void *data = NULL;
			if(doWrite) {
				data = gridPatch_getWindowedDataCopy(patch, i, idxLo, idxHi, NULL);
				for(int k=0; k<NDIM; k++) {
					dimsPatch[k]=idxHi[k]-idxLo[k]+1;
					idxLo[k]=idxLo[k]-w->rtwLo[k];
				}
			} else {
				for(int k=0; k<NDIM; k++)
					idxLo[k]=0;
			}
			
			#ifdef WITH_MPI
				transProps = H5Pcreate(H5P_DATASET_XFER);
				assert(transProps >= 0);
				H5Pset_dxpl_mpio(transProps, H5FD_MPIO_COLLECTIVE);
			#endif
		
			dataSpaceFile = H5Dget_space(dataSet);
			assert(dataSpaceFile >= 0);
			if (local_checkIfPatchIsCompleteGrid(gridSize, dimsPatch))
				gridUtilHDF5_selectHyperslab(dataSpaceFile, NULL, dimsPatch);
			else {
				gridUtilHDF5_selectHyperslab(dataSpaceFile, idxLo, dimsPatch);
			}
			dataSpacePatch = gridUtilHDF5_getDataSpaceFromDims(dimsPatch);
			
			//if(doWrite) {
				H5Dwrite(dataSet, dt, dataSpacePatch, dataSpaceFile,
			         transProps, data);
			//}
		
			H5Sclose(dataSpacePatch);
			H5Sclose(dataSpaceFile);
			if (transProps != H5P_DEFAULT)
				H5Pclose(transProps);

				
				
				//local_writeVariableAtPatch(var, patchToWrite, dataSet, dt, gridSize);
		}
		H5Dclose(dataSet);
	}
	H5Sclose(gridSize);
	
	printf("done");
}


static void
local_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid)
{
	gridWriterHDF5_t w = (gridWriterHDF5_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_HDF5);
	assert(grid != NULL);
	assert(w->base.isActive);

	int               numVars, numPatches;
	gridPointUint32_t dims;
	hid_t             gridSize, dsCreationPropList;

	numVars    = gridRegular_getNumVars(grid);
	numPatches = gridRegular_getNumPatches(grid);

	gridRegular_getDims(grid, dims);
	gridSize           = gridUtilHDF5_getDataSpaceFromDims(dims);
	dsCreationPropList = local_getDSCreationPropList(w);

	for (int i = 0; i < numVars; i++) {
		dataVar_t var     = gridRegular_getVarHandle(grid, i);
		hid_t     dt      = dataVar_getHDF5Datatype(var);
		hid_t     dataSet = H5Dcreate(w->fileHandle, dataVar_getName(var),
		                              dt, gridSize, H5P_DEFAULT,
		                              dsCreationPropList,
		                              H5P_DEFAULT);
		for (int j = 0; j < numPatches; j++) {
			gridPatch_t patch = gridRegular_getPatchHandle(grid, j);
			assert(w->fileHandle != H5I_INVALID_HID);
			local_writeVariableAtPatch(var, patch, dataSet, dt, gridSize);
		}
		H5Dclose(dataSet);
	}
	H5Sclose(gridSize);
}


#ifdef WITH_MPI
static hid_t
local_getAccessPropsFileAccessMPI(MPI_Comm comm)
{
	hid_t accessProp;
	int   rtn;

	accessProp = H5Pcreate(H5P_FILE_ACCESS);
	if (accessProp < 0)
		diediedie(EXIT_FAILURE);
	rtn = H5Pset_fapl_mpio(accessProp, comm, MPI_INFO_NULL);
	if (rtn < 0)
		diediedie(EXIT_FAILURE);

	return accessProp;
}

#endif

static hid_t
local_getFileHandle(const gridWriterHDF5_t writer)
{
	hid_t fileHandle;
#ifdef WITH_MPI
	hid_t accessProp      = local_getAccessPropsFileAccessMPI(
	    writer->mpiComm);
#else
	hid_t      accessProp = H5P_DEFAULT;
#endif
	const char *fname     = filename_getFullName(writer->base.fileName);

	if (!gridWriter_hasBeenActivated((gridWriter_t)writer)) {
		if (gridWriter_getOverwriteFileIfExists((gridWriter_t)writer)) {
			fileHandle = H5Fcreate(fname, H5F_ACC_TRUNC, H5P_DEFAULT,
			                       accessProp);
		} else {
			fileHandle = H5Fcreate(fname, H5F_ACC_EXCL, H5P_DEFAULT,
			                       accessProp);
		}
	} else {
		fileHandle = H5Fopen(fname, H5F_ACC_RDWR, accessProp);
	}
	if (fileHandle < 0)
		diediedie(EXIT_FAILURE);

	if (accessProp != H5P_DEFAULT)
		H5Pclose(accessProp);

	return fileHandle;
}

static hid_t
local_getDSCreationPropList(const gridWriterHDF5_t writer)
{
	hid_t rtn = H5P_DEFAULT;

	if (writer->doChunking) {
		herr_t err;
		rtn = H5Pcreate(H5P_DATASET_CREATE);
		assert(rtn >= 0);

		err = H5Pset_chunk(rtn, NDIM, writer->chunkSize);
		if (err < 0)
			diediedie(EXIT_FAILURE);
		err = H5Pset_alloc_time(rtn,H5D_ALLOC_TIME_INCR);
		if (err < 0)
			diediedie(EXIT_FAILURE);

		if (writer->doChecksum) {
			err = H5Pset_filter(rtn, H5Z_FILTER_FLETCHER32,
			                    H5Z_FLAG_MANDATORY, 0, NULL);
			if (err < 0)
				diediedie(EXIT_FAILURE);
		}
		if (writer->doCompression) {
			if (writer->compressionFilter == H5Z_FILTER_DEFLATE)
				H5Pset_deflate(rtn, 9);
			if (err < 0)
				diediedie(EXIT_FAILURE);
		}
	}

	return rtn;
}

inline static void
local_writeVariableAtPatch(dataVar_t   var,
                           gridPatch_t patch,
                           hid_t       dataSet,
                           hid_t       dt,
                           hid_t       gridSize)
{
	hid_t             transProps = H5P_DEFAULT;
	gridPointUint32_t dimsPatch;
	hid_t             dataSpacePatch, dataSpaceFile;
	void              *data = gridPatch_getVarDataHandleByVar(patch, var);

#ifdef WITH_MPI
	transProps = H5Pcreate(H5P_DATASET_XFER);
	assert(transProps >= 0);
	H5Pset_dxpl_mpio(transProps, H5FD_MPIO_COLLECTIVE);
#endif

	dataSpaceFile = H5Dget_space(dataSet);
	assert(dataSpaceFile >= 0);
	gridPatch_getDims(patch, dimsPatch);
	if (local_checkIfPatchIsCompleteGrid(gridSize, dimsPatch))
		gridUtilHDF5_selectHyperslab(dataSpaceFile, NULL, dimsPatch);
	else {
		gridPointUint32_t idxLo;
		gridPatch_getIdxLo(patch, idxLo);
		gridUtilHDF5_selectHyperslab(dataSpaceFile, idxLo, dimsPatch);
	}
	dataSpacePatch = gridUtilHDF5_getDataSpaceFromDims(dimsPatch);

	H5Dwrite(dataSet, dt, dataSpacePatch, dataSpaceFile,
	         transProps, data);

	H5Sclose(dataSpacePatch);
	H5Sclose(dataSpaceFile);
	if (transProps != H5P_DEFAULT)
		H5Pclose(transProps);
} /* local_writeVariableAtPatch */

inline static void
local_writeNull() {
	hid_t             transProps = H5P_DEFAULT;
#ifdef WITH_MPI
	transProps = H5Pcreate(H5P_DATASET_XFER);
	assert(transProps >= 0);
	H5Pset_dxpl_mpio(transProps, H5FD_MPIO_COLLECTIVE);
#endif
	if (transProps != H5P_DEFAULT)
		H5Pclose(transProps);
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
