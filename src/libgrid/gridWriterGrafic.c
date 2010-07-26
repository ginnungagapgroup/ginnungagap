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
#  include <pmpio.h>
#endif
#include "gridVar.h"
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


static void *
local_createFile(const char *fname, const char *dname, void *udata);


#ifdef WITH_MPI
static void *
local_openFile(const char     *fname,
               const char     *dname,
               PMPIO_iomode_t iomode,
               void           *udata);

#endif

static void
local_closeFile(void *file, void *udata);

static graficFormat_t
local_getGraficTypeFromGridType(const gridVar_t var);


/*--- Implementations of exported functios ------------------------------*/
extern gridWriterGrafic_t
gridWriterGrafic_new(const char *prefix, bool isWhiteNoise)
{
	gridWriterGrafic_t writer;

	assert(prefix != NULL);

	writer              = xmalloc(sizeof(struct gridWriterGrafic_struct));
	writer->type        = IO_TYPE_GRAFIC;
	writer->func        = (gridWriter_func_t)&local_func;
	writer->grafic      = grafic_new(isWhiteNoise);
	writer->isActive    = false;
	writer->curOutFile  = 0;
	writer->numOutFiles = LOCAL_NUMOUTFILES;
	writer->fileNames   = local_getFileNames(prefix, writer->numOutFiles);
#ifdef WITH_MPI
	writer->baton       = NULL;
	writer->groupRank   = -1;
	writer->rankInGroup = -1;
	writer->globalRank  = -1;
#endif

	return writer;
}

extern gridWriterGrafic_t
gridWriterGrafic_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridWriterGrafic_t writer;
	bool               isWhiteNoise;
	uint32_t           *size = NULL;
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
	assert(tmp->type == IO_TYPE_GRAFIC);

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
	if (tmp->baton != NULL)
		PMPIO_Finish(tmp->baton);
#endif
	xfree(*writer);

	*writer = NULL;
}

extern void
gridWriterGrafic_activate(gridWriter_t writer)
{
	gridWriterGrafic_t tmp = (gridWriterGrafic_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == IO_TYPE_GRAFIC);
#ifdef WITH_MPI
	assert(tmp->baton != NULL);
#endif

	if (!tmp->isActive) {
#ifdef WITH_MPI
		    (void) PMPIO_WaitForBaton(tmp->baton,
		                              tmp->fileNames[tmp->curOutFile],
		                              NULL);
#else
		(void)local_createFile(tmp->fileNames[tmp->curOutFile], NULL, tmp);
#endif
		tmp->isActive = true;
	}
}

extern void
gridWriterGrafic_deactivate(gridWriter_t writer)
{
	gridWriterGrafic_t tmp = (gridWriterGrafic_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == IO_TYPE_GRAFIC);
#ifdef WITH_MPI
	assert(tmp->baton != NULL);
#endif

	if (tmp->isActive) {
#ifdef WITH_MPI
		PMPIO_HandOffBaton(tmp->baton, tmp);
#else
		local_closeFile(tmp, tmp);
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
	gridVar_t          var;
	int                numComponents;
	gridPointUint32_t  dims;
	gridPointUint32_t  idxLo;
	graficFormat_t     format;

	assert(tmp != NULL);
	assert(tmp->type == IO_TYPE_GRAFIC);
	assert(tmp->isActive);
	assert(tmp->grafic != NULL);
	assert(patch != NULL);
	assert(patchName != NULL);

	if (patchName == NULL || origin == NULL || delta == NULL)
		;

	gridPatch_getIdxLo(patch, idxLo);
	gridPatch_getDims(patch, dims);
	var           = gridPatch_getVarHandle(patch, 0);
	data          = gridPatch_getVarDataHandle(patch, 0);
	numComponents = gridVar_getNumComponents(var);
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
	assert(((gridWriterGrafic_t)writer)->type == IO_TYPE_GRAFIC);
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
	int                rank;

	assert(tmp != NULL);
	assert(tmp->type == IO_TYPE_GRAFIC);

	MPI_Comm_rank(mpiComm, &rank);

	tmp->baton = PMPIO_Init(1, PMPIO_WRITE, mpiComm,
	                        LOCAL_MPI_TAG,
	                        &local_createFile, &local_openFile,
	                        &local_closeFile, (void *)tmp);

	tmp->groupRank   = PMPIO_GroupRank(tmp->baton, rank);
	tmp->rankInGroup = PMPIO_RankInGroup(tmp->baton, rank);
	tmp->globalRank  = rank;
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

static void *
local_createFile(const char *fname, const char *dname, void *udata)
{
	gridWriterGrafic_t writer = (gridWriterGrafic_t)udata;

	if (fname != NULL || dname != NULL)
		; // Only used to swat compiler warning

	grafic_setFileName(writer->grafic,
	                   writer->fileNames[writer->curOutFile]);
	grafic_makeEmptyFile(writer->grafic);

	return writer;
}

#ifdef WITH_MPI
static void *
local_openFile(const char     *fname,
               const char     *dname,
               PMPIO_iomode_t iomode,
               void           *udata)
{
	gridWriterGrafic_t writer = (gridWriterGrafic_t)udata;

	if (fname != NULL || dname != NULL)
		; // Only used to swat compiler warning

	if (iomode == PMPIO_READ) {
		diediedie(EXIT_FAILURE);
	}

	grafic_setFileName(writer->grafic, writer->fileNames[writer->curOutFile]);

	return writer;
}

#endif

static void
local_closeFile(void *file, void *udata)
{
	if (file != NULL || udata != NULL)
		; // Only used to swat compiler warning
}

static graficFormat_t
local_getGraficTypeFromGridType(const gridVar_t var)
{
	graficFormat_t varType;

	switch (gridVar_getType(var)) {
	case GRIDVARTYPE_DOUBLE:
		varType = GRAFIC_FORMAT_DOUBLE;
		break;
	case GRIDVARTYPE_FPV:
		varType = sizeof(fpv_t) == 4 ?
		          GRAFIC_FORMAT_FLOAT : GRAFIC_FORMAT_DOUBLE;
		break;
	case GRIDVARTYPE_INT:
	default:
		diediedie(EXIT_FAILURE);
	}

	return varType;
}
