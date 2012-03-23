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
#include "gridReaderBov.h"
#ifdef WITH_HDF5
#  include "gridReaderHDF5.h"
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

	return reader;
}
#endif

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
