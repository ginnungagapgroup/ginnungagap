// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file gridWriterFactory.c
 * @ingroup  GROUP
 * @brief  SHORT DESC
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterFactory.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"
#include "gridIO.h"
#include "gridWriterGrafic.h"
#ifdef WITH_SILO
#  include "gridWriterSilo.h"
#  include <silo.h>
#  include <string.h>
#endif
#ifdef WITH_HDF5
#  include "gridWriterHDF5.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridWriter_t
local_newFromIniWrapper(parse_ini_t ini,
                        const char  *base,
                        const char  *extended);

static gridIO_type_t
local_getType(parse_ini_t ini, const char *secName);

static filename_t
local_getFileName(parse_ini_t ini, const char *secName);

static bool
local_getOverwrite(parse_ini_t ini, const char *secName);

static gridWriter_t
local_getWriter(parse_ini_t ini, const char *secName, gridIO_type_t type);


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
		getFromIni(&tmp, parse_ini_get_double, ini, "dx", sectionName);
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
	bool             tmp, doChunking, doChecksum, doCompression;


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


	return (gridWriter_t)writer;
} /* gridWriterFactory_newFromIniHDF5 */

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

	fn        = local_getFileName(ini, base);
	overwrite = local_getOverwrite(ini, base);
	type      = local_getType(ini, extended);

	writer    = local_getWriter(ini, extended, type);

	gridWriter_overlayFileName(writer, fn);
	gridWriter_setOverwriteFileIfExists(writer, overwrite);

	filename_del(&fn);

	return writer;
}

static gridIO_type_t
local_getType(parse_ini_t ini, const char *secName)
{
	char          *typeName;
	gridIO_type_t type;

	getFromIni(&typeName, parse_ini_get_string, ini, "type", secName);
	type = gridIO_getTypeFromName(typeName);
	xfree(typeName);

	return type;
}

static filename_t
local_getFileName(parse_ini_t ini, const char *secName)
{
	filename_t fn;
	char       *path      = NULL;
	char       *prefix    = NULL;
	char       *qualifier = NULL;
	char       *suffix    = NULL;

	parse_ini_get_string(ini, "path", secName, &path);
	getFromIni(&prefix, parse_ini_get_string, ini, "prefix", secName);
	parse_ini_get_string(ini, "qualifier", secName, &qualifier);
	parse_ini_get_string(ini, "suffix", secName, &suffix);

	fn = filename_newFull(path, prefix, qualifier, suffix);

	xfree(prefix);
	if (path != NULL)
		xfree(path);
	if (qualifier != NULL)
		xfree(qualifier);
	if (suffix != NULL)
		xfree(suffix);

	return fn;
}

static bool
local_getOverwrite(parse_ini_t ini, const char *secName)
{
	bool overwrite;

	if (!parse_ini_get_bool(ini, "overwriteFileIfExists", secName,
	                        &overwrite))
		overwrite = false;

	return overwrite;
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
