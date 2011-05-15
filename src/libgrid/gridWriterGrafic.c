// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterGrafic.h"
#include <assert.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#  include "../libutil/groupi.h"
#endif
#include "../libdata/dataVar.h"
#include "gridRegular.h"
#include "gridPatch.h"
#include "../libutil/parse_ini.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridWriterGrafic_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define LOCAL_NUMOUTFILES 4
#ifdef WITH_MPI
#  define LOCAL_MPI_TAG 987
#endif

/*--- Local variables ---------------------------------------------------*/
static const char *local_outFileEndings[LOCAL_NUMOUTFILES]
    = {"delta", "velx", "vely", "velz"};

static struct gridWriter_func_struct local_func
    = {&gridWriterGrafic_del,
	   &gridWriterGrafic_activate,
	   &gridWriterGrafic_deactivate,
	   &gridWriterGrafic_writeGridPatch,
	   &gridWriterGrafic_writeGridRegular,
#ifdef WITH_MPI
	   &gridWriterGrafic_initParallel
#endif
	};

/*--- Prototypes of local functions -------------------------------------*/
static char **
local_getFileNames(const char *prefix, int numFiles);

inline static void
local_setFileNameInGrafic(gridWriterGrafic_t writer);

inline static void
local_createEmptyGraficFile(gridWriterGrafic_t writer);

static graficFormat_t
local_getGraficTypeFromGridType(const dataVar_t var);


#ifdef WITH_MPI
static void *
local_acquireFunc(int seqId, int seqNum, int seqLen, void *data);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern gridWriterGrafic_t
gridWriterGrafic_new(const char *prefix, bool isWhiteNoise)
{
	gridWriterGrafic_t writer;

	assert(prefix != NULL);

	writer              = xmalloc(sizeof(struct gridWriterGrafic_struct));
	writer->type        = GRIDIO_TYPE_GRAFIC;
	writer->func        = (gridWriter_func_t)&local_func;
	writer->grafic      = grafic_new(isWhiteNoise);
	writer->isActive    = false;
	writer->curOutFile  = 0;
	writer->numOutFiles = LOCAL_NUMOUTFILES;
	writer->fileNames   = local_getFileNames(prefix, writer->numOutFiles);
#ifdef WITH_MPI
	writer->groupi      = NULL;
#endif

	return writer;
}

extern gridWriterGrafic_t
gridWriterGrafic_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridWriterGrafic_t writer;
	bool               isWhiteNoise;
	uint32_t           *size   = NULL;
	char               *prefix = NULL;


	assert(ini != NULL);
	assert(sectionName != NULL);

	getFromIni(&isWhiteNoise, parse_ini_get_bool, ini,
	           "isWhiteNoise", sectionName);
	getFromIni(&prefix, parse_ini_get_string, ini, "prefix", sectionName);
	if (!parse_ini_get_int32list(ini, "size", sectionName, 3,
	                             (int32_t **)&size)) {
		fprintf(stderr, "FATAL:  Could not get size from section %s.\n",
		        sectionName);
		exit(EXIT_FAILURE);
	}

	writer = gridWriterGrafic_new(prefix, isWhiteNoise);
	grafic_setSize(writer->grafic, size);
	xfree(size);
	if (grafic_isWhiteNoise(writer->grafic)) {
		int iseed;
		getFromIni(&iseed, parse_ini_get_int32, ini, "iseed", sectionName);
		grafic_setIseed(writer->grafic, iseed);
	} else {
		double tmp;
		float  xOff[3] = {0.0, 0.0, 0.0};
		getFromIni(&tmp, parse_ini_get_double, ini, "dx", sectionName);
		grafic_setDx(writer->grafic, (float)tmp);
		grafic_setXoff(writer->grafic, xOff);
		getFromIni(&tmp, parse_ini_get_double, ini, "astart", sectionName);
		grafic_setAstart(writer->grafic, (float)tmp);
		getFromIni(&tmp, parse_ini_get_double, ini, "omegam", sectionName);
		grafic_setOmegam(writer->grafic, (float)tmp);
		getFromIni(&tmp, parse_ini_get_double, ini, "omegav", sectionName);
		grafic_setOmegav(writer->grafic, (float)tmp);
		getFromIni(&tmp, parse_ini_get_double, ini, "h0", sectionName);
		grafic_setH0(writer->grafic, (float)tmp);
	}

	xfree(prefix);

	return writer;
} /* gridWriterGrafic_newFromIni */

extern void
gridWriterGrafic_del(gridWriter_t *writer)
{
	gridWriterGrafic_t tmp;

	assert(writer != NULL && *writer != NULL);
	tmp = (gridWriterGrafic_t)*writer;
	assert(tmp->type == GRIDIO_TYPE_GRAFIC);

	if (tmp->isActive)
		gridWriterGrafic_deactivate(*writer);
	if (tmp->fileNames != NULL) {
		for (int i = 0; i < tmp->numOutFiles; i++) {
			if (tmp->fileNames[i] != NULL)
				xfree(tmp->fileNames[i]);
		}
		xfree(tmp->fileNames);
	}
	grafic_del(&(tmp->grafic));
#ifdef WITH_MPI
	if (tmp->groupi != NULL)
		groupi_del(&(tmp->groupi));
#endif
	xfree(*writer);

	*writer = NULL;
}

extern void
gridWriterGrafic_activate(gridWriter_t writer)
{
	gridWriterGrafic_t tmp = (gridWriterGrafic_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_GRAFIC);
#ifdef WITH_MPI
	assert(tmp->groupi != NULL);
#endif

	if (!tmp->isActive) {
#ifdef WITH_MPI
		    (void) groupi_acquire(tmp->groupi);
#else
		local_setFileNameInGrafic(tmp);
		local_createEmptyGraficFile(tmp);
#endif
		tmp->isActive = true;
	}
}

extern void
gridWriterGrafic_deactivate(gridWriter_t writer)
{
	gridWriterGrafic_t tmp = (gridWriterGrafic_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_GRAFIC);
#ifdef WITH_MPI
	assert(tmp->groupi != NULL);
#endif

	if (tmp->isActive) {
#ifdef WITH_MPI
		groupi_release(tmp->groupi);
#endif
		tmp->isActive    = false;
		tmp->curOutFile++;
		tmp->curOutFile %= tmp->numOutFiles;
	}
}

extern void
gridWriterGrafic_writeGridPatch(gridWriter_t   writer,
                                gridPatch_t    patch,
                                const char     *patchName,
                                gridPointDbl_t origin,
                                gridPointDbl_t delta)
{
	gridWriterGrafic_t tmp = (gridWriterGrafic_t)writer;
	void               *data;
	dataVar_t          var;
	int                numComponents;
	gridPointUint32_t  dims;
	gridPointUint32_t  idxLo;
	graficFormat_t     format;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_GRAFIC);
	assert(tmp->isActive);
	assert(tmp->grafic != NULL);
	assert(patch != NULL);
	assert(patchName != NULL);

	if ((patchName == NULL) || (origin == NULL) || (delta == NULL))
		;

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);
	var           = gridPatch_getVarHandle(patch, 0);
	data          = gridPatch_getVarDataHandle(patch, 0);
	numComponents = dataVar_getNumComponents(var);
	format        = local_getGraficTypeFromGridType(var);

	grafic_writeWindowed(tmp->grafic, data, format, numComponents,
	                     idxLo, dims);
}

extern void
gridWriterGrafic_writeGridRegular(gridWriter_t  writer,
                                  gridRegular_t grid)
{
	gridPatch_t patch;

	assert(writer != NULL);
	assert(((gridWriterGrafic_t)writer)->type == GRIDIO_TYPE_GRAFIC);
	assert(((gridWriterGrafic_t)writer)->isActive);
	assert(grid != NULL);

	patch = gridRegular_getPatchHandle(grid, 0);
	gridWriterGrafic_writeGridPatch(writer, patch, "null",
	                                NULL, NULL);
}

#ifdef WITH_MPI
extern void
gridWriterGrafic_initParallel(gridWriter_t writer, MPI_Comm mpiComm)
{
	gridWriterGrafic_t tmp = (gridWriterGrafic_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_GRAFIC);

	tmp->groupi = groupi_new(1, mpiComm, LOCAL_MPI_TAG,
	                         GROUPI_MODE_BLOCK);
	groupi_registerAcquireFunc(tmp->groupi, &local_acquireFunc, tmp);
}

#endif


/*--- Implementations of local functions --------------------------------*/
static char **
local_getFileNames(const char *prefix, int numFiles)
{
	char **fnames;

	fnames = xmalloc(sizeof(char *) * numFiles);
	for (int i = 0; i < numFiles; i++)
		fnames[i] = xstrmerge(prefix, local_outFileEndings[i]);

	return fnames;
}

inline static void
local_setFileNameInGrafic(gridWriterGrafic_t writer)
{
	grafic_setFileName(writer->grafic,
	                   writer->fileNames[writer->curOutFile]);
}

inline static void
local_createEmptyGraficFile(gridWriterGrafic_t writer)
{
	grafic_makeEmptyFile(writer->grafic);
}

static graficFormat_t
local_getGraficTypeFromGridType(const dataVar_t var)
{
	graficFormat_t varType;

	switch (dataVar_getType(var)) {
	case DATAVARTYPE_DOUBLE:
		varType = GRAFIC_FORMAT_DOUBLE;
		break;
	case DATAVARTYPE_FPV:
		varType = sizeof(fpv_t) == 4 ?
		          GRAFIC_FORMAT_FLOAT : GRAFIC_FORMAT_DOUBLE;
		break;
	case DATAVARTYPE_INT:
	default:
		diediedie(EXIT_FAILURE);
	}

	return varType;
}

#ifdef WITH_MPI
static void *
local_acquireFunc(int seqId, int seqNum, int seqLen, void *data)
{
	gridWriterGrafic_t writer = (gridWriterGrafic_t)data;

	assert(writer != NULL);

	if ((seqId < 0) || (seqNum < 0) || (seqLen < 0))
		; // Only used to swat compiler warnings.

	local_setFileNameInGrafic(writer);
	if (groupi_isFirstInGroup(writer->groupi))
		local_createEmptyGraficFile(writer);

	return writer;
}

#endif
