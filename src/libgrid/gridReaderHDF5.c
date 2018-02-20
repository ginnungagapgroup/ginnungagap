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
 #define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
 #define MAX(X,Y) ((X) > (Y) ? (X) : (Y))

/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the functions table for the HDF5 reader. */
static struct gridReader_func_struct local_func
    = {&gridReaderHDF5_del,
	   &gridReaderHDF5_readIntoPatch,
	   &gridReaderHDF5_readIntoPatchForVar};

/*--- Prototypes of local functions -------------------------------------*/
static void
local_handleFilenameChange(gridReader_t reader);
static void
local_readIntoPatchForVar_old(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar);
static void
local_readIntoPatchForVar_doPatch(gridReader_t reader,
                                   gridPatch_t  patch,
                                   int          idxOfVar);

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
	if(reader->doPatch) {
		local_readIntoPatchForVar_doPatch(reader, patch, idxOfVar);
	} else {
		local_readIntoPatchForVar_old(reader, patch, idxOfVar);
	}
	
} 

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
	gridReader_setDoPatch((gridReader_t)reader,false);
	gridReader_setDims((gridReader_t)reader, 100000000);
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
static void
local_handleFilenameChange(gridReader_t reader)
{
	assert(reader != NULL);
	assert(reader->type == GRIDIO_TYPE_HDF5);

	const char *fileName = filename_getFullName(reader->fileName);

	int res = H5Fis_hdf5(fileName);
	if (res <= 0) {
		if (res == 0) {
			fprintf(stderr, "ERROR: %s does not seem to be an HDF5 file.\n",
			        fileName);
		} else {
			fprintf(stderr,
			        "ERROR: Failed to check whether %s is a HDF5 file.\n",
			        fileName);
		}
		diediedie(EXIT_FAILURE);
	}

	hid_t file = H5Fopen(fileName, H5F_ACC_RDONLY, H5P_DEFAULT);
	if (file < 0) {
		fprintf(stderr, "ERROR: Could not open %s for reading.\n", fileName);
		diediedie(EXIT_FAILURE);
	}

	gridReaderHDF5_setH5File((gridReaderHDF5_t)reader, file);
}

/* gridReaderHDF5_readIntoPatchForVar */

static void
local_memUsage() {
  unsigned long used;
  char s[80];
  FILE *f = fopen("/proc/self/statm","r");
  fscanf(f,"%ld",&used);
  printf("Used: %ld\n",used);
  fclose(f);
  
  f = fopen("/proc/meminfo","r");
  fgets(s,80,f);
  printf("%s\n",s);
  fgets(s,80,f);
  printf("%s\n",s);
  fclose(f);
}

static void
local_readIntoPatchForVar_old(gridReader_t reader,
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
	//local_memUsage();
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
	
	//local_memUsage();
	
	if (H5Tequal(dataTypeFile, dataTypePatch)) {
		H5Dread(dataSet, dataTypeFile, dataSpacePatch,
		        dataSpaceFile, H5P_DEFAULT, data);
	} else {
		fprintf(stderr, "ERROR: Datatype in memory differs from file.\n");
		diediedie(EXIT_FAILURE);
	}
//	local_memUsage();

	H5Sclose(dataSpacePatch);
	H5Tclose(dataTypePatch);
	H5Sclose(dataSpaceFile);
	H5Tclose(dataTypeFile);
	H5Dclose(dataSet);
}



#define read() \
{ \
	doRead=true; \
	for(int k=0; k<NDIM; k++) { \
		if(reader->doPatch) { \
			idxLoRead[k]=MAX(idxLoW[k], idxLoPatch[k]); \
			idxLoReadRtw[k]=idxLoRead[k]-reader->rtwLo[k]; \
			idxLoReadRtw[k] = idxLoReadRtw[k]<0 ? idxLoReadRtw[k]+period[k] : idxLoReadRtw[k]; \
			idxLoReadRtw[k] = idxLoReadRtw[k]>=period[k] ? idxLoReadRtw[k]-period[k] : idxLoReadRtw[k]; \
			idxHiRead[k]=MIN(idxLoPatch[k]+dimsPatch[k]-1, idxHiW[k]); \
			if(idxHiRead[k]<idxLoRead[k]) doRead=false; \
		} else { \
			idxLoRead[k]=idxLoPatch[k]; \
			idxLoReadRtw[k]=idxLoPatch[k]; \
			idxHiRead[k]=idxLoRead[k]+dimsPatch[k]-1; \
		} \
		dimsRead[k]=idxHiRead[k]-idxLoRead[k]+1; \
	} \
	if (doRead) { \
		if(!doRead) { \
			for(int k=0; k<NDIM; k++) { \
				dimsRead[k]=0; \
				idxHiRead[k]=idxLoRead[k]+dimsRead[k]-1; \
			} \
		} \
		dataSet        = H5Dopen(((gridReaderHDF5_t)reader)->file, \
								 dataVar_getName(var), H5P_DEFAULT); \
		dataTypeFile   = H5Dget_type(dataSet); \
		dataSpaceFile  = H5Dget_space(dataSet); \
		dataTypePatch  = dataVar_getHDF5Datatype(var); \
		dataSpacePatch = gridUtilHDF5_getDataSpaceFromDims(dimsRead); \
		gridUtilHDF5_selectHyperslab(dataSpaceFile, idxLoReadRtw, dimsRead); \
		if (H5Tequal(dataTypeFile, dataTypePatch)) { \
			H5Dread(dataSet, dataTypeFile, dataSpacePatch, \
					dataSpaceFile, H5P_DEFAULT, data); \
		} else { \
			fprintf(stderr, "ERROR: Datatype in memory differs from file.\n"); \
			diediedie(EXIT_FAILURE); \
		} \
		H5Sclose(dataSpacePatch); \
		H5Tclose(dataTypePatch); \
		H5Sclose(dataSpaceFile); \
		H5Tclose(dataTypeFile); \
		H5Dclose(dataSet); \
		gridPatch_allocateVarData(patch,idxOfVar); \
		gridPatch_putWindowedData(patch, idxOfVar, idxLoRead, idxHiRead, data); \
	} \
}
	
	
	

static void
local_readIntoPatchForVar_doPatch(gridReader_t reader,
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
	gridPointUint32_t idxLoPatch, dimsPatch, idxLoRead, dimsRead, idxHiRead, idxLoReadRtw;
	gridPointUint32_t period, rtwHi;
	int32_t 		  idxLo1[3], idxLo2[3], idxHi1[3], idxHi2[3], idxLoW[2], idxHiW[3];
	dataVar_t         var   = gridPatch_getVarHandle(patch, idxOfVar);
	void             *data;
	bool              doRead;
	//void             *dataPatch = gridPatch_getVarDataHandle(patch, idxOfVar);
	uint64_t  numCellsToAllocate = 1;
	numCellsToAllocate = gridPatch_getNumCellsActual(patch,
	                                                 idxOfVar);
	data               = dataVar_getMemory(var, numCellsToAllocate);
	
	for(int k=0; k<NDIM; k++) {
		period[k] = reader->gridDims[k];
		rtwHi[k] = reader->rtwLo[k] + reader->rtwDims[k]-1;
	}
	
	gridPatch_getIdxLo(patch, idxLoPatch);
	gridPatch_getDims(patch, dimsPatch);
	
	for(int k=0; k<NDIM; k++) {
		idxLo1[k] = reader->rtwLo[k]<0 ? reader->rtwLo[k]+period[k] : reader->rtwLo[k];
		idxHi1[k] = MIN(rtwHi[k], period[k]-1);
		idxHi1[k] = reader->rtwLo[k]<0 ? period[k]-1 : idxHi1[k];
		idxLo2[k] = rtwHi[k]>=period[k] ? 0 : rtwHi[k];
		idxLo2[k] = reader->rtwLo[k]<0 ? 0 : idxLo2[k];
		idxHi2[k] = rtwHi[k]>=period[k] ? rtwHi[k]-period[k] : rtwHi[k];
		if (idxHi2[k] == idxLo2[k]) idxHi2[k] -= 1;
	}
	
	idxLoW[0] = idxLo1[0];
	idxLoW[1] = idxLo1[1];
	idxLoW[2] = idxLo1[2];
	idxHiW[0] = idxHi1[0];
	idxHiW[1] = idxHi1[1];
	idxHiW[2] = idxHi1[2];
	read();
	
	idxLoW[0] = idxLo2[0];
	idxLoW[1] = idxLo1[1];
	idxLoW[2] = idxLo1[2];
	idxHiW[0] = idxHi2[0];
	idxHiW[1] = idxHi1[1];
	idxHiW[2] = idxHi1[2];
	read();
	
	idxLoW[0] = idxLo1[0];
	idxLoW[1] = idxLo2[1];
	idxLoW[2] = idxLo1[2];
	idxHiW[0] = idxHi1[0];
	idxHiW[1] = idxHi2[1];
	idxHiW[2] = idxHi1[2];
	read();
	
	idxLoW[0] = idxLo1[0];
	idxLoW[1] = idxLo1[1];
	idxLoW[2] = idxLo2[2];
	idxHiW[0] = idxHi1[0];
	idxHiW[1] = idxHi1[1];
	idxHiW[2] = idxHi2[2];
	read();
	
	idxLoW[0] = idxLo2[0];
	idxLoW[1] = idxLo2[1];
	idxLoW[2] = idxLo1[2];
	idxHiW[0] = idxHi2[0];
	idxHiW[1] = idxHi2[1];
	idxHiW[2] = idxHi1[2];
	read();
	
	idxLoW[0] = idxLo1[0];
	idxLoW[1] = idxLo2[1];
	idxLoW[2] = idxLo2[2];
	idxHiW[0] = idxHi1[0];
	idxHiW[1] = idxHi2[1];
	idxHiW[2] = idxHi2[2];
	read();
	
	idxLoW[0] = idxLo2[0];
	idxLoW[1] = idxLo1[1];
	idxLoW[2] = idxLo2[2];
	idxHiW[0] = idxHi2[0];
	idxHiW[1] = idxHi1[1];
	idxHiW[2] = idxHi2[2];
	read();
	
	idxLoW[0] = idxLo2[0];
	idxLoW[1] = idxLo2[1];
	idxLoW[2] = idxLo2[2];
	idxHiW[0] = idxHi2[0];
	idxHiW[1] = idxHi2[1];
	idxHiW[2] = idxHi2[2];
	read();
	
	dataVar_freeMemory(var, data);
	
} 

#undef read
