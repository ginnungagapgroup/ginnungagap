// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsFactory.c
 * @ingroup  toolsGICSFactory
 * @brief  Provides the implementations to the factory methods to create
 *         @c generateICs applications.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICsFactory.h"
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <math.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_OPENMP
#  include <omp.h>
#endif
#include "generateICs.h"
#include "../../src/libgrid/gridReaderFactory.h"
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libdata/dataVar.h"
#include "../../src/libcosmo/cosmo.h"
#include "../../src/libcosmo/cosmoModel.h"
#include "../../src/libg9p/g9pHierarchy.h"
#include "../../src/libg9p/g9pHierarchyIO.h"
#include "../../src/libg9p/g9pMask.h"
#include "../../src/libg9p/g9pMaskIO.h"
#include "../../src/libutil/parse_ini.h"
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/diediedie.h"


/*--- Local structure ---------------------------------------------------*/

/** @brief  Small helper structure to map the content of the ini file.  */
struct generateICs_IniData_struct {
	/** @brief  Stores key @c boxsizeInMpch. */
	double boxsizeInMpch;
	/** @brief  Stores key @c zInit. */
	double zInit;
	/** @brief  Stores key @c doGas. */
	bool   doGas;
	/** @brief  Stores key @c doLongIDs. */
	bool   doLongIDs;
	/** @brief  Stores key @c outputSection. */
	char   *outputSection;
	/** @brief  Stores key @c cosmologySection. */
	char   *cosmologySection;
	/** @brief  Stores key @c hierarchySection. */
	char   *hierarchySection;
	/** @brief  Stores key @c maskSection. */
	char   *maskSection;
};

/** @brief  Short name for a reference to the helper structure.  */
typedef struct generateICs_IniData_struct *generateICs_iniData_t;


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Helper function to read the main section from an ini file.
 *
 * @param[in,out]  ini
 *                    The ini file to use.  Must be a valid ini file,
 *                    passing @c NULL is undefined.
 * @param[in]      sectionName
 *                    The name of the section from which to read the
 *                    information.  Must be a valid section name, passing
 *                    @C NULL is undefined.
 *
 * @return  Returns a new structure holding the information from the main
 *          section in the ini file.
 */
static generateICs_iniData_t
local_iniDataNewFromIni(parse_ini_t ini, const char *sectionName);


/**
 * @brief  Helper function to initialze the structure.
 *
 * @param[in,out]  iniData
 *                    The structure to initialize, passing @c NULL is
 *                    undefined.
 *
 * @return  Returns nothing.
 */
static void
local_iniDataInit(generateICs_iniData_t iniData);


/**
 * @brief  Helper function to delete the structure.
 *
 * @param[in,out]  *iniData
 *                    Pointer to the external variable holding the reference
 *                    to the structure that should be deleted.  After
 *                    deletion, the external variable will be set to
 *                    @c NULL.
 *
 * @return  Returns nothing.
 */
static void
local_iniDataDel(generateICs_iniData_t *iniData);


/**
 * @brief  Helper function for local_iniDataNewFromIni().
 *
 * @param[in,out]  iniData
 *                    The structure to fill.
 * @param[in,out]  ini
 *                    The ini file from which to read.  Must be a valid ini
 *                    file object, passing @c NULL is undefined.
 * @param[in]      secName
 *                    The name of the section from which to read.  Must be a
 *                    valid section name, passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
inline static void
local_iniDataNewFromIni_boxsize(generateICs_iniData_t iniData,
                                parse_ini_t           ini,
                                const char            *secName);


/** @copydoc local_iniDataNewFromIni_boxsize() */
inline static void
local_iniDataNewFromIni_zInit(generateICs_iniData_t iniData,
                              parse_ini_t           ini,
                              const char            *secName);


/** @copydoc local_iniDataNewFromIni_boxsize() */
inline static void
local_iniDataNewFromIni_doGas(generateICs_iniData_t iniData,
                              parse_ini_t           ini,
                              const char            *secName);


/** @copydoc local_iniDataNewFromIni_boxsize() */
inline static void
local_iniDataNewFromIni_doLongIDs(generateICs_iniData_t iniData,
                                  parse_ini_t           ini,
                                  const char            *secName);


/** @copydoc local_iniDataNewFromIni_boxsize() */
inline static void
local_iniDataNewFromIni_section(generateICs_iniData_t iniData,
                                parse_ini_t           ini,
                                const char            *secName);


/**
 * @brief  Helper function for generateICsFactory_newFromIni() dealing with
 *         the output.
 *
 * @param[in,out]  ini
 *                    The ini file to work with.
 * @param[in]      *levelSection
 *                    The name of the section from which to construct the
 *                    readers.
 * @param[in,out]  genics
 *                    The object to work with.
 *
 * @return  Returns nothing.
 */
inline static void
local_newFromIni_output(parse_ini_t   ini,
                        const char    *secName,
                        generateICs_t genics);


/*--- Implementations of exported functios ------------------------------*/
extern generateICs_t
generateICsFactory_newFromIni(parse_ini_t ini, const char *sectionName)
{
	generateICs_iniData_t iniData;
	generateICs_t         genics;

	assert(ini != NULL);

	iniData = local_iniDataNewFromIni(ini,
	                                  (sectionName != NULL) ? sectionName :
	                                  GENERATEICSCONFIG_DEFAULT_SECTIONNAME);

	genics = generateICs_new();
	generateICs_setBoxsizeInMpch(genics, iniData->boxsizeInMpch);
	generateICs_setAInit(genics, cosmo_z2a(iniData->zInit));
	generateICs_setDoGas(genics, iniData->doGas);
	generateICs_setDoLongIDs(genics, iniData->doLongIDs);

	cosmoModel_t model;
	model = cosmoModel_newFromIni(ini, iniData->cosmologySection);
	generateICs_setCosmoModel(genics, model);

	g9pHierarchy_t hierarchy;
	hierarchy = g9pHierarchyIO_newFromIni(ini, iniData->hierarchySection);
	generateICs_setHierarchy(genics, hierarchy);

	g9pMask_t mask;
	mask = g9pMaskIO_newFromIni(ini, iniData->maskSection,
	                            generateICs_getHierarchy(genics));
	generateICs_setMask(genics, mask);

	local_newFromIni_output(ini, iniData->outputSection, genics);

	local_iniDataDel(&iniData);

	return genics;
}

/*--- Implementations of local functions --------------------------------*/

static generateICs_iniData_t
local_iniDataNewFromIni(parse_ini_t ini, const char *sectionName)
{
	generateICs_iniData_t iniData;

	iniData = xmalloc(sizeof(struct generateICs_IniData_struct));
	local_iniDataInit(iniData);

	local_iniDataNewFromIni_boxsize(iniData, ini, sectionName);
	local_iniDataNewFromIni_zInit(iniData, ini, sectionName);
	local_iniDataNewFromIni_doGas(iniData, ini, sectionName);
	local_iniDataNewFromIni_doLongIDs(iniData, ini, sectionName);
	local_iniDataNewFromIni_section(iniData, ini, sectionName);

	return iniData;
}

static void
local_iniDataInit(generateICs_iniData_t iniData)
{
	assert(iniData != NULL);
	iniData->boxsizeInMpch    = 0.0;
	iniData->zInit            = 0.0;
	iniData->doGas            = false;
	iniData->doLongIDs        = false;
	iniData->outputSection    = NULL;
	iniData->cosmologySection = NULL;
	iniData->maskSection      = NULL;
}

static void
local_iniDataDel(generateICs_iniData_t *iniData)
{
	assert(iniData != NULL);
	assert(*iniData != NULL);

	if ((*iniData)->outputSection != NULL)
		xfree((*iniData)->outputSection);
	if ((*iniData)->cosmologySection != NULL)
		xfree((*iniData)->cosmologySection);
	if ((*iniData)->maskSection != NULL)
		xfree((*iniData)->maskSection);

	xfree(*iniData);
}

inline static void
local_iniDataNewFromIni_boxsize(generateICs_iniData_t iniData,
                                parse_ini_t           ini,
                                const char            *secName)
{
	assert(iniData != NULL);
	assert(ini != NULL);
	assert(secName != NULL);

	getFromIni(&(iniData->boxsizeInMpch), parse_ini_get_double,
	           ini, "boxsizeInMpch", secName);
}

inline static void
local_iniDataNewFromIni_zInit(generateICs_iniData_t iniData,
                              parse_ini_t           ini,
                              const char            *secName)
{
	assert(iniData != NULL);
	assert(ini != NULL);
	assert(secName != NULL);

	getFromIni(&(iniData->zInit), parse_ini_get_double,
	           ini, "zInit", secName);
}

inline static void
local_iniDataNewFromIni_doGas(generateICs_iniData_t iniData,
                              parse_ini_t           ini,
                              const char            *secName)
{
	assert(iniData != NULL);
	assert(ini != NULL);
	assert(secName != NULL);

	if (!parse_ini_get_bool(ini, "doGas", secName,
	                        &(iniData->doGas))) {
		iniData->doGas = GENERATEICSCONFIG_DEFAULT_DOGAS;
	}
}

inline static void
local_iniDataNewFromIni_doLongIDs(generateICs_iniData_t iniData,
                                  parse_ini_t           ini,
                                  const char            *secName)
{
	assert(iniData != NULL);
	assert(ini != NULL);
	assert(secName != NULL);

	if (!parse_ini_get_bool(ini, "doLongIDs", secName,
	                        &(iniData->doLongIDs))) {
		iniData->doLongIDs = GENERATEICSCONFIG_DEFAULT_DOLONGIDS;
	}
}

inline static void
local_iniDataNewFromIni_section(generateICs_iniData_t iniData,
                                parse_ini_t           ini,
                                const char            *secName)
{
	assert(iniData != NULL);
	assert(ini != NULL);
	assert(secName != NULL);

	if (!parse_ini_get_string(ini, "outputSection", secName,
	                          &(iniData->outputSection))) {
		iniData->outputSection = xstrdup(
		    GENERATEICSCONFIG_DEFAULT_OUTPUTSECTION);
	}
	if (!parse_ini_get_string(ini, "cosmologySection", secName,
	                          &(iniData->cosmologySection))) {
		iniData->cosmologySection
		    = xstrdup(GENERATEICSCONFIG_DEFAULT_COSMOLOGYSECTION);
	}
	if (!parse_ini_get_string(ini, "hierarchySection", secName,
	                          &(iniData->hierarchySection))) {
		iniData->hierarchySection = xstrdup(
		    GENERATEICSCONFIG_DEFAULT_HIERARCHYSECTION);
	}
	if (!parse_ini_get_string(ini, "maskSection", secName,
	                          &(iniData->maskSection))) {
		iniData->maskSection = xstrdup(
		    GENERATEICSCONFIG_DEFAULT_MASKSECTION);
	}
}

inline static void
local_newFromIni_output(parse_ini_t   ini,
                        const char    *secName,
                        generateICs_t genics)
{
	uint32_t numFiles;
	char     *prefix;

	getFromIni(&numFiles, parse_ini_get_uint32, ini, "numFiles", secName);
	getFromIni(&prefix, parse_ini_get_string, ini, "prefix", secName);

	generateICs_setNumFiles(genics, numFiles);
	generateICs_setPrefix(genics, prefix);
}
