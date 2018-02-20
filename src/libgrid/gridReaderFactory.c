// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridReaderFactory.c
 * @ingroup  libgridIOInFactory
 * @brief  Provides functions to contruct grid reader.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridReaderFactory.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "gridIO.h"
#include "gridIOCommon.h"
#include "gridReaderGrafic.h"
#include "gridReaderGrafic_adt.h"
#include "gridReaderBov.h"
#ifdef WITH_HDF5
#  include "gridReaderHDF5.h"
#  include "gridReaderHDF5_adt.h"
#  include <string.h>
#endif
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Wrapper to create a new reader.
 *
 * @param[in,out]  ini
 *                    The ini file to use.  Passing @c NULL is undefind.
 * @param[in]      *base
 *                    The section from which to read the basic information.
 * @param[in]      *extended
 *                    The section from which to read the extended
 *                    information (file format specific information).  This
 *                    may be the same as @c base.
 *
 * @return  Returns a new grid reader constructed from the information in
 *          the ini file.
 */
static gridReader_t
local_newFromIniWrapper(parse_ini_t ini,
                        const char  *base,
                        const char  *extended);
                        
static void
local_doPatch(parse_ini_t ini, const char *sectionName, gridReader_t reader);


/*--- Implementations of exported functions -----------------------------*/
extern gridReader_t
gridReaderFactory_newReaderFromIni(parse_ini_t ini, const char *sectionName)
{
	gridReader_t reader;
	char         *extended;

	assert(ini != NULL);
	assert(sectionName != NULL);

	if (parse_ini_get_string(ini, "readerSection", sectionName,
	                         &extended)) {
		reader = local_newFromIniWrapper(ini, sectionName, extended);
		xfree(extended);
	} else {
		reader = local_newFromIniWrapper(ini, sectionName, sectionName);
	}

	return reader;
}

extern gridReaderGrafic_t
gridReaderFactory_newFromIniGrafic(parse_ini_t ini,
                                   const char  *sectionName,
                                   filename_t  fn)
{
	gridReaderGrafic_t reader;
	bool tmp, doPatch;

	reader = gridReaderGrafic_new();

	if (fn == GRIDREADERFACTORY_GET_FILENAME_FROM_SPECIFIC_SECTION) {
		fn = gridIOCommon_getFileName(ini, sectionName, false);
	} else {
		filename_t fnSpecific = gridIOCommon_getFileName(ini, sectionName,
		                                                 true);
		filename_copySetFields(fn, fnSpecific);
		filename_del(&fnSpecific);
	}

	gridReader_setFileName((gridReader_t)reader, fn);
	
	tmp = parse_ini_get_bool(ini, "doPatch", sectionName,
							 &doPatch);
	if (tmp && doPatch) {
		local_doPatch(ini, sectionName, &reader->base);
	}

	return reader;
}

extern gridReaderBov_t
gridReaderFactory_newFromIniBov(parse_ini_t ini,
                                const char  *sectionName,
                                filename_t  fn)
{
	gridReaderBov_t reader;

	reader = gridReaderBov_new();

	if (fn == GRIDREADERFACTORY_GET_FILENAME_FROM_SPECIFIC_SECTION) {
		fn = gridIOCommon_getFileName(ini, sectionName, false);
	} else {
		filename_t fnSpecific = gridIOCommon_getFileName(ini, sectionName,
		                                                 true);
		filename_copySetFields(fn, fnSpecific);
		filename_del(&fnSpecific);
	}

	gridReader_setFileName((gridReader_t)reader, fn);

	return reader;
}

#ifdef WITH_HDF5
extern gridReaderHDF5_t
gridReaderFactory_newFromIniHDF5(parse_ini_t ini,
                                 const char  *sectionName,
                                 filename_t  fn)
{
	gridReaderHDF5_t reader;
	bool tmp, doPatch;

	reader = gridReaderHDF5_new();

	if (fn == GRIDREADERFACTORY_GET_FILENAME_FROM_SPECIFIC_SECTION) {
		fn = gridIOCommon_getFileName(ini, sectionName, false);
	} else {
		filename_t fnSpecific = gridIOCommon_getFileName(ini, sectionName,
		                                                 true);
		filename_copySetFields(fn, fnSpecific);
		filename_del(&fnSpecific);
	}

	gridReader_setFileName((gridReader_t)reader, fn);
	
	tmp = parse_ini_get_bool(ini, "doPatch", sectionName,
							 &doPatch);
	if (tmp && doPatch) {
		local_doPatch(ini, sectionName, &reader->base);
	}

	return reader;
}
#endif

static void
local_doPatch(parse_ini_t ini, const char *sectionName, gridReader_t reader)
{
	char*		patchSection;
	char*		unit;
	bool		tmp;
	
	int32_t  patchLo[3];
	gridPointUint32_t patchDims;
	gridReader_setDoPatch(reader, true);
	
	tmp = parse_ini_get_string(ini, "patchSection", sectionName,
			&patchSection);
	if(!tmp) {
		fprintf(stderr, "Could not get patchSection from section %s.\n",
		        sectionName);
		diediedie(EXIT_FAILURE);
	}
			
	tmp = parse_ini_get_string(ini, "unit", patchSection,
			&unit);
	if(!tmp) {
		fprintf(stderr, "Could not get unit from section %s.\n",
		        patchSection);
		diediedie(EXIT_FAILURE);
	}
	
	if(strcmp(unit,"cells")==0) {
		int32_t           *dataFile;
		if (!parse_ini_get_int32list(ini, "patchLo", patchSection,
		                             NDIM, (int32_t **)&dataFile)) {
			fprintf(stderr, "Could not get patchLo from section %s.\n",
			        patchSection);
			diediedie(EXIT_FAILURE);
		}
		for (int i = 0; i < NDIM; i++)
			patchLo[i] = dataFile[i];
		
		if (!parse_ini_get_int32list(ini, "patchDims", patchSection,
		                             NDIM, (int32_t **)&dataFile)) {
			fprintf(stderr, "Could not get patchDims from section %s.\n",
			        patchSection);
			diediedie(EXIT_FAILURE);
		}
		for (int i = 0; i < NDIM; i++)
			patchDims[i] = dataFile[i];
			
		xfree(dataFile);
	}
	
	if(strcmp(unit,"Mpch")==0) {
		double   	box;
		int32_t  	dim1D;
		double*		dataFile;
		
		parse_ini_get_double(ini, "boxsizeInMpch", patchSection,
						&box);
		parse_ini_get_int32(ini, "dim1D", patchSection,
						&dim1D);
						
		if (!parse_ini_get_doublelist(ini, "patchLo", patchSection,
		                             NDIM, (double **)&dataFile)) {
			fprintf(stderr, "Could not get patchLo from section %s.\n",
			        patchSection);
			diediedie(EXIT_FAILURE);
		}
		for (int i = 0; i < NDIM; i++)
			patchLo[i] = (int32_t) (dataFile[i]/box*dim1D);
		
		if (!parse_ini_get_doublelist(ini, "patchDims", patchSection,
		                             NDIM, (double **)&dataFile)) {
			fprintf(stderr, "Could not get patchDims from section %s.\n",
			        patchSection);
			diediedie(EXIT_FAILURE);
		}
		for (int i = 0; i < NDIM; i++)
			patchDims[i] = (int32_t) (dataFile[i]/box*dim1D);
		
		xfree(dataFile);
	}
	
	if(strcmp(unit,"cells")!=0 && strcmp(unit,"Mpch")!=0) {
		fprintf(stderr, "Unit can be only cells or Mpch in section %s.\n",
			        patchSection);
			diediedie(EXIT_FAILURE);
	}
	
	int32_t  	dim1D;
	if(!parse_ini_get_int32(ini, "dim1D", patchSection,
						&dim1D))     dim1D = 10000000;
	gridReader_setDims(reader,dim1D);
	
	int rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	if (rank == 0) {
		printf("\nPatch will be read.\n");
		printf("Patch Lo, cells: %i %i %i\n", patchLo[0], patchLo[1], patchLo[2]);
		printf("Patch Dims, cells: %i %i %i\n\n", patchDims[0], patchDims[1], patchDims[2]);
	}
	gridReader_setRtw(reader, patchLo, patchDims);
}



/*--- Implementations of local functions --------------------------------*/
static gridReader_t
local_newFromIniWrapper(parse_ini_t ini,
                        const char  *base,
                        const char  *extended)
{
	gridReader_t  r;
	gridIO_type_t type;
	filename_t    fn;

	fn   = gridIOCommon_getFileName(ini, base, true);
	type = gridIOCommon_getType(ini, base);

	if (type == GRIDIO_TYPE_BOV) {
		r = (gridReader_t)gridReaderFactory_newFromIniBov(ini, extended, fn);
	} else if (type == GRIDIO_TYPE_GRAFIC) {
		r = (gridReader_t)gridReaderFactory_newFromIniGrafic(ini, extended,
		                                                     fn);
	} else if (type == GRIDIO_TYPE_HDF5) {
#ifdef WITH_HDF5
		r = (gridReader_t)gridReaderFactory_newFromIniHDF5(ini, extended,
		                                                   fn);
#else
		fprintf(stderr,
		        "To use HF5 input, run configure using --with-hdf5\n");
		diediedie(EXIT_FAILURE);
#endif
	} else {
		fprintf(stderr, "Cannot create reader for %s\n",
		        gridIO_getNameFromType(type));
		diediedie(EXIT_FAILURE);
	}

	return r;
}
