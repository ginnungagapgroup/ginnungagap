// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file gridWriterFactory.c
 * @ingroup  libgridIOOutFactory
 * @brief  Provides functions to contruct grid writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterFactory.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "gridIOCommon.h"
#include "gridWriterGrafic.h"
#ifdef WITH_SILO
#  include "gridWriterSilo.h"
#  include <silo.h>
#  include <string.h>
#endif
#ifdef WITH_HDF5
#  include "gridWriterHDF5.h"
#  include <string.h>
#endif
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Wrapper to create a new writer.
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
 * @return  Returns a new grid writer constructed from the information in
 *          the ini file.
 */
static gridWriter_t
local_newFromIniWrapper(parse_ini_t ini,
                        const char  *base,
                        const char  *extended);


/**
 * @brief  This will call the according factory routine for the request
 *         writer type.
 *
 * @param[in,out]  ini
 *                    The ini file to use.  Passing @c NULL is undefined.
 * @param[in]      *secName
 *                    The section in which to look for the construction
 *                    information.
 * @param[in]      type
 *                    The type of the writer.
 *
 * @return  Returns the new writer of the requested type.
 */
static gridWriter_t
local_getWriter(parse_ini_t ini, const char *secName, gridIO_type_t type);

#ifdef WITH_HDF5
static void
local_doPatch(parse_ini_t ini, const char *sectionName, gridWriterHDF5_t writer);
#endif

/*--- Implementations of exported functions -----------------------------*/
extern gridWriter_t
gridWriterFactory_newWriterFromIni(parse_ini_t ini, const char *sectionName)
{
	gridWriter_t writer;
	char         *extended;

	assert(ini != NULL);
	assert(sectionName != NULL);


	if (parse_ini_get_string(ini, "writerSection", sectionName,
	                         &extended)) {
		writer = local_newFromIniWrapper(ini, sectionName, extended);
		xfree(extended);
	} else {
		writer = local_newFromIniWrapper(ini, sectionName, sectionName);
	}

	return writer;
}

extern gridWriter_t
gridWriterFactory_newFromIniGrafic(parse_ini_t ini, const char *sectionName)
{
	gridWriterGrafic_t writer;
	grafic_t           grafic;
	bool               isWhiteNoise;
	uint32_t           *size = NULL;


	assert(ini != NULL);
	assert(sectionName != NULL);

	writer = gridWriterGrafic_new();
	grafic = gridWriterGrafic_getGrafic(writer);

	if (parse_ini_get_bool(ini, "isWhiteNoise", sectionName, &isWhiteNoise))
		grafic_setIsWhiteNoise(grafic, isWhiteNoise);

	if (!parse_ini_get_int32list(ini, "size", sectionName, 3,
	                             (int32_t **)&size)) {
		fprintf(stderr, "FATAL:  Could not get size from section %s.\n",
		        sectionName);
		exit(EXIT_FAILURE);
	}
	grafic_setSize(grafic, size);
	xfree(size);

	if (grafic_getIsWhiteNoise(grafic)) {
		int iseed;
		getFromIni(&iseed, parse_ini_get_int32, ini, "iseed", sectionName);
		grafic_setIseed(grafic, iseed);
	} else {
		double tmp;
		float  xOff[3] = {0.0, 0.0, 0.0};
		uint32_t	*patchLo = NULL;
		getFromIni(&tmp, parse_ini_get_double, ini, "dx", sectionName);
		if (parse_ini_get_int32list(ini, "patchLo", sectionName, 3,
	                             (int32_t **)&patchLo)) {
			for(int i=0;i<NDIM;i++)
				xOff[i] = patchLo[i] * (float)tmp;
		}
		grafic_setDx(grafic, (float)tmp);
		grafic_setXoff(grafic, xOff);
		getFromIni(&tmp, parse_ini_get_double, ini, "astart", sectionName);
		grafic_setAstart(grafic, (float)tmp);
		getFromIni(&tmp, parse_ini_get_double, ini, "omegam", sectionName);
		grafic_setOmegam(grafic, (float)tmp);
		getFromIni(&tmp, parse_ini_get_double, ini, "omegav", sectionName);
		grafic_setOmegav(grafic, (float)tmp);
		getFromIni(&tmp, parse_ini_get_double, ini, "h0", sectionName);
		grafic_setH0(grafic, (float)tmp);
	}

	return (gridWriter_t)writer;
} /* gridWriterFactory_newFromIniGrafic */

#ifdef WITH_HDF5
extern gridWriter_t
gridWriterFactory_newFromIniHDF5(parse_ini_t ini, const char *sectionName)
{
	assert(ini != NULL);
	assert(sectionName != NULL);

	gridWriterHDF5_t writer;
	bool             tmp, doChunking, doChecksum, doCompression, doPatch;


	writer = gridWriterHDF5_new();
	tmp    = parse_ini_get_bool(ini, "doChunking", sectionName, &doChunking);
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
	
	tmp = parse_ini_get_bool(ini, "doPatch", sectionName,
							 &doPatch);
	if (tmp && doPatch) {
		local_doPatch(ini, sectionName, writer);
	}


	return (gridWriter_t)writer;
} /* gridWriterFactory_newFromIniHDF5 */

static void
local_doPatch(parse_ini_t ini, const char *sectionName, gridWriterHDF5_t writer)
{
	char*		patchSection;
	char*		unit;
	bool		tmp;
	
	
	int32_t patchLo[3];
	gridPointUint32_t patchDims;
	gridWriterHDF5_setDoPatch(writer, true);
	
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
		
		parse_ini_get_double(ini, "boxsizeInMpch", "Ginnungagap",
						&box);
		parse_ini_get_int32(ini, "dim1D", "Ginnungagap",
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
	int rank = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	if (rank == 0) {
		printf("\nPatch will be written for all files.\n");
		printf("Patch Lo, cells: %i %i %i\n", patchLo[0], patchLo[1], patchLo[2]);
		printf("Patch Dims, cells: %i %i %i\n\n", patchDims[0], patchDims[1], patchDims[2]);
	}
	
	gridWriterHDF5_setRtw(writer, patchLo, patchDims);
}
#endif

#ifdef WITH_SILO
extern gridWriter_t
gridWriterFactory_newFromIniSilo(parse_ini_t ini, const char *sectionName)
{
	assert(ini != NULL);
	assert(sectionName != NULL);

	gridWriterSilo_t writer = gridWriterSilo_new();
	char             *dbTypeStr;
	int              dbType;
	int              numFiles;


	if (parse_ini_get_string(ini, "dbType", sectionName, &dbTypeStr)) {
		if (strcmp(dbTypeStr, "DB_PDB") == 0)
			dbType = DB_PDB;
		else if (strcmp(dbTypeStr, "DB_HDF5") == 0)
			dbType = DB_HDF5;
		else {
			fprintf(stderr, "Unknown or unsupported Silo DB type: %s\n",
			        dbTypeStr);
			diediedie(EXIT_FAILURE);
		}
		xfree(dbTypeStr);
		gridWriterSilo_setDbType(writer, dbType);
	}

	if (parse_ini_get_int32(ini, "numFiles", sectionName, &numFiles))
		gridWriterSilo_setNumFiles(writer, numFiles);

	return (gridWriter_t)writer;
}

#endif

/*--- Implementations of local functions --------------------------------*/

static gridWriter_t
local_newFromIniWrapper(parse_ini_t ini,
                        const char  *base,
                        const char  *extended)
{
	gridWriter_t  writer;
	gridIO_type_t type;
	filename_t    fn;
	bool          overwrite;

	fn        = gridIOCommon_getFileName(ini, base, false);
	overwrite = gridIOCommon_getOverwrite(ini, base);
	type      = gridIOCommon_getType(ini, base);

	writer    = local_getWriter(ini, extended, type);

	gridWriter_overlayFileName(writer, fn);
	gridWriter_setOverwriteFileIfExists(writer, overwrite);

	filename_del(&fn);

	return writer;
}

static gridWriter_t
local_getWriter(parse_ini_t ini, const char *secName, gridIO_type_t type)
{
	gridWriter_t writer;

	if (type == GRIDIO_TYPE_SILO) {
#ifdef WITH_SILO
		writer = gridWriterFactory_newFromIniSilo(ini, secName);
#else
		fprintf(stderr,
		        "To use Silo output, run configure using --with-silo\n");
		diediedie(EXIT_FAILURE);
#endif
	} else if (type == GRIDIO_TYPE_GRAFIC) {
		writer = gridWriterFactory_newFromIniGrafic(ini, secName);
	} else if (type == GRIDIO_TYPE_HDF5) {
#ifdef WITH_HDF5
		writer = gridWriterFactory_newFromIniHDF5(ini, secName);
#else
		fprintf(stderr,
		        "To use HF5 output, run configure using --with-hdf5\n");
		diediedie(EXIT_FAILURE);
#endif
	} else {
		fprintf(stderr, "Cannot create writer for %s\n",
		        gridIO_getNameFromType(type));
		diediedie(EXIT_FAILURE);
	}

	return writer;
}
