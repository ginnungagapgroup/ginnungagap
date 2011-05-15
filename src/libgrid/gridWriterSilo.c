// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#ifdef WITH_SILO
#  include "gridWriterSilo.h"
#  include <assert.h>
#  include <string.h>
#  include <silo.h>
#  ifdef WITH_MPI
#    include <mpi.h>
#    include <pmpio.h>
#  endif
#  include "../libutil/xmem.h"
#  include "../libutil/xstring.h"
#  include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#  include "gridWriterSilo_adt.h"


/*--- Local defines -----------------------------------------------------*/
#  define LOCAL_FILESUFFIX ".silo"
#  ifdef WITH_MPI
#    define LOCAL_DIRPREFIX     "domain_"
#    define LOCAL_NUMFILEDIGITS 3
#    define LOCAL_NUMDIRDIGITS  5
#    define LOCAL_MPI_TAG       987
#  endif

/*--- Local variables ---------------------------------------------------*/
static struct gridWriter_func_struct local_func
    = { &gridWriterSilo_del,
	    &gridWriterSilo_activate,
	    &gridWriterSilo_deactivate,
	    &gridWriterSilo_writeGridPatch,
	    &gridWriterSilo_writeGridRegular,
#  ifdef WITH_MPI
	    &gridWriterSilo_initParallel
#  endif
	};

/*--- Prototypes of local functions -------------------------------------*/
static void *
local_createDB(const char *fname, const char *dname, void *udata);


#  ifdef WITH_MPI
static void *
local_openDB(const char     *fname,
             const char     *dname,
             PMPIO_iomode_t iomode,
             void           *udata);

#  endif

static void
local_closeDB(void *file, void *udata);

static void
local_createFileName(gridWriterSilo_t writer);

static void
local_createDirName(gridWriterSilo_t writer);

static char *
local_getGridName(int rankInGroup, const char *base);

static char **
local_getPatchNames(const char *base, int numPatches);

static void
local_delPatchNames(char **patchNames, int numPatches);

static void
local_writePatchData(gridWriterSilo_t writer,
                     gridPatch_t      patch,
                     const char       *patchName);

inline static int
local_getVarType(dataVar_t var);

static void *
local_getPatchVarName(dataVar_t var, const char *patchName, char *varName);

static bool
local_dirExistsInFile(DBfile *db, const char *dname);


/*--- Implementations of exported functios ------------------------------*/
extern gridWriterSilo_t
gridWriterSilo_new(const char *prefix, int dbType)
{
	gridWriterSilo_t writer;

	assert(prefix != NULL);
#  ifdef WITH_HDF5
	assert(dbType == DB_HDF5 || dbType == DB_PDB);
#  else
	assert(dbType == DB_PDB);
#  endif

	writer              = xmalloc(sizeof(struct gridWriterSilo_struct));
	writer->type        = GRIDIO_TYPE_SILO;
	writer->func        = (gridWriter_func_t)&local_func;
	writer->isActive    = false;
#  ifdef WITH_MPI
	writer->baton       = NULL;
	writer->groupRank   = -1;
	writer->rankInGroup = -1;
	writer->globalRank  = -1;
#  endif
	writer->prefix      = xstrdup(prefix);
	writer->dbType      = dbType;
	writer->numFiles    = 1;
	writer->f           = NULL;
	writer->fileName    = NULL;
	writer->dirName     = NULL;

	return writer;
}

extern gridWriterSilo_t
gridWriterSilo_newFromIni(parse_ini_t ini, const char *sectionName)
{
	gridWriterSilo_t writer;
	char             *dbTypeStr;
	char             *prefix;
	int              dbType;

	assert(ini != NULL);
	assert(sectionName != NULL);

	getFromIni(&dbTypeStr, parse_ini_get_string,
	           ini, "dbType", sectionName);
	getFromIni(&prefix, parse_ini_get_string,
	           ini, "prefix", sectionName);

	if (strcmp(dbTypeStr, "DB_PDB") == 0)
		dbType = DB_PDB;
#  ifdef WITH_HDF5
	else if (strcmp(dbTypeStr, "DB_HDF5") == 0)
		dbType = DB_HDF5;
#  endif
	else {
		fprintf(stderr, "Unknown or unsupported Silo DB type: %s\n",
		        dbTypeStr);
		diediedie(EXIT_FAILURE);
	}

	writer = gridWriterSilo_new(prefix, dbType);
	getFromIni(&(writer->numFiles), parse_ini_get_int32,
	           ini, "numFiles", sectionName);

	xfree(prefix);
	xfree(dbTypeStr);

	return writer;
} /* gridWriterSilo_newFromIni */

extern void
gridWriterSilo_del(gridWriter_t *writer)
{
	gridWriterSilo_t tmp;

	assert(writer != NULL && *writer != NULL);
	tmp = (gridWriterSilo_t)*writer;
	assert(tmp->type == GRIDIO_TYPE_SILO);

	if (tmp->isActive)
		gridWriterSilo_deactivate(*writer);

	if (tmp->fileName != NULL)
		xfree(tmp->fileName);
	if (tmp->dirName != NULL)
		xfree(tmp->dirName);
	xfree(tmp->prefix);
#  ifdef WITH_MPI
	if (tmp->baton != NULL)
		PMPIO_Finish(tmp->baton);
#  endif
	xfree(*writer);

	*writer = NULL;
}

extern void
gridWriterSilo_activate(gridWriter_t writer)
{
	gridWriterSilo_t tmp = (gridWriterSilo_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_SILO);
#  ifdef WITH_MPI
	assert(tmp->baton != NULL);
#  endif

	if (!tmp->isActive) {
		local_createFileName(tmp);
		local_createDirName(tmp);
#  ifdef WITH_MPI
		tmp->f = PMPIO_WaitForBaton(tmp->baton, tmp->fileName,
		                            tmp->dirName);
#  else
		tmp->f = local_createDB(tmp->fileName, tmp->dirName,
		                        tmp);
#  endif
		tmp->isActive = true;
	}
}

extern void
gridWriterSilo_deactivate(gridWriter_t writer)
{
	gridWriterSilo_t tmp = (gridWriterSilo_t)writer;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_SILO);
#  ifdef WITH_MPI
	assert(tmp->baton != NULL);
#  endif

	if (tmp->isActive) {
#  ifdef WITH_MPI
		PMPIO_HandOffBaton(tmp->baton, tmp->f);
#  else
		local_closeDB(tmp->f, tmp);
#  endif
		tmp->isActive = false;
	}
}

extern void
gridWriterSilo_writeGridPatch(gridWriter_t   writer,
                              gridPatch_t    patch,
                              const char     *patchName,
                              gridPointDbl_t origin,
                              gridPointDbl_t delta)
{
	gridWriterSilo_t  tmp = (gridWriterSilo_t)writer;
	double            *coords[NDIM];
	gridPointInt_t    dims;
	gridPointUint32_t idxLo;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_SILO);
	assert(tmp->isActive);
	assert(patch != NULL);
	assert(patchName != NULL);

	gridPatch_getIdxLo(patch, idxLo);

	for (int i = 0; i < NDIM; i++) {
		// need left and right coords of the cell, hence the +1
		dims[i]   = (int)(gridPatch_getOneDim(patch, i) + 1);
		coords[i] = xmalloc(sizeof(double) * dims[i]);
		for (uint32_t j = 0; j < dims[i]; j++)
			coords[i][j] = origin[i] + (j + idxLo[i]) * delta[i];
	}

	DBPutQuadmesh(tmp->f, patchName, NULL, coords, dims, NDIM,
	              DB_DOUBLE, DB_COLLINEAR, NULL);

	for (int i = 0; i < NDIM; i++)
		xfree(coords[i]);

	local_writePatchData(tmp, patch, patchName);
}

extern void
gridWriterSilo_writeGridRegular(gridWriter_t  writer,
                                gridRegular_t grid)
{
	gridWriterSilo_t tmp = (gridWriterSilo_t)writer;
	int              numPatches;
	gridPointDbl_t   origin, delta;
	char             **patchNames;
	char             *gridName;
	int              *meshTypes;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_SILO);
	assert(tmp->isActive);
	assert(grid != NULL);

#  ifdef WITH_MPI
	gridName = local_getGridName(tmp->rankInGroup,
	                             gridRegular_getName(grid));
#  else
	gridName   = local_getGridName(0, gridRegular_getName(grid));
#  endif
	numPatches = gridRegular_getNumPatches(grid);
	meshTypes  = xmalloc(sizeof(int) * numPatches);
	patchNames = local_getPatchNames(gridName, numPatches);
	gridRegular_getOrigin(grid, origin);
	gridRegular_getDelta(grid, delta);
	for (int i = 0; i < numPatches; i++) {
		gridPatch_t patch = gridRegular_getPatchHandle(grid, i);
		gridWriterSilo_writeGridPatch(writer, patch, patchNames[i],
		                              origin, delta);
		meshTypes[i] = DB_QUAD_RECT;
	}
	DBPutMultimesh(tmp->f, gridName, numPatches, patchNames,
	               meshTypes, NULL);

	xfree(meshTypes);
	local_delPatchNames(patchNames, numPatches);
	xfree(gridName);
} /* gridWriterSilo_writeGridRegular */

#  ifdef WITH_MPI
extern void
gridWriterSilo_initParallel(gridWriter_t writer, MPI_Comm mpiComm)
{
	gridWriterSilo_t tmp = (gridWriterSilo_t)writer;
	int              rank;

	assert(tmp != NULL);
	assert(tmp->type == GRIDIO_TYPE_SILO);

	MPI_Comm_rank(mpiComm, &rank);

	tmp->baton = PMPIO_Init(tmp->numFiles, PMPIO_WRITE, mpiComm,
	                        LOCAL_MPI_TAG,
	                        &local_createDB, &local_openDB,
	                        &local_closeDB, (void *)tmp);

	tmp->groupRank   = PMPIO_GroupRank(tmp->baton, rank);
	tmp->rankInGroup = PMPIO_RankInGroup(tmp->baton, rank);
	tmp->globalRank  = rank;
}

#  endif


/*--- Implementations of local functions --------------------------------*/
static void *
local_createDB(const char *fname, const char *dname, void *udata)
{
	FILE             *f;
	gridWriterSilo_t writer = (gridWriterSilo_t)udata;

	f = fopen(fname, "r");
	if (f != NULL) {
		fclose(f);
		writer->f = DBOpen(fname, writer->dbType, DB_APPEND);
	} else {
		writer->f = DBCreate(fname, DB_CLOBBER, DB_LOCAL, NULL,
		                     writer->dbType);
	}
	if ((dname != NULL) && (dname[0] != '\0')) {
		if (!local_dirExistsInFile(writer->f, dname))
			DBMkDir(writer->f, dname);
		DBSetDir(writer->f, dname);
	}

	return writer->f;
}

#  ifdef WITH_MPI
static void *
local_openDB(const char     *fname,
             const char     *dname,
             PMPIO_iomode_t iomode,
             void           *udata)
{
	gridWriterSilo_t writer = (gridWriterSilo_t)udata;
	int              mode   = DB_APPEND;

	if (iomode == PMPIO_READ)
		mode = DB_READ;
	if (writer->f == NULL) {
		writer->f = DBOpen(fname, writer->dbType, mode);
		if ((dname != NULL) && (dname[0] != '\0')) {
			if (mode == DB_APPEND) {
				if (!local_dirExistsInFile(writer->f, dname))
					DBMkDir(writer->f, dname);
				DBSetDir(writer->f, dname);
			}
		}
	}

	return writer->f;
}

#  endif

static void
local_closeDB(void *file, void *udata)
{
	gridWriterSilo_t writer = (gridWriterSilo_t)udata;

	assert((DBfile *)file == writer->f);

	DBClose((DBfile *)file);
	writer->f = NULL;
}

static void
local_createFileName(gridWriterSilo_t writer)
{
	size_t charsToAlloc = 1;

	if (writer->fileName != NULL)
		xfree(writer->fileName);

	charsToAlloc    += strlen(writer->prefix);
	charsToAlloc    += strlen(LOCAL_FILESUFFIX);
#  ifdef WITH_MPI
	charsToAlloc    += 1 + LOCAL_NUMFILEDIGITS;
#  endif
	writer->fileName = xmalloc(charsToAlloc * sizeof(char));
#  ifdef WITH_MPI
	sprintf(writer->fileName, "%s_%0*i%s",
	        writer->prefix, LOCAL_NUMFILEDIGITS,
	        writer->groupRank, LOCAL_FILESUFFIX);
#  else
	sprintf(writer->fileName, "%s%s", writer->prefix, LOCAL_FILESUFFIX);
#  endif
}

static void
local_createDirName(gridWriterSilo_t writer)
{
	size_t charsToAlloc = 0;

	if (writer->dirName != NULL)
		xfree(writer->dirName);

	charsToAlloc    = 1;
#  ifdef WITH_MPI
	charsToAlloc   += strlen(LOCAL_DIRPREFIX) + LOCAL_NUMDIRDIGITS;
#  endif
	writer->dirName = xmalloc(charsToAlloc * sizeof(char));
#  ifdef WITH_MPI
	sprintf(writer->dirName, "%s%0*i",
	        LOCAL_DIRPREFIX, LOCAL_NUMDIRDIGITS, writer->globalRank);
#  else
	writer->dirName[0] = '\0';
#  endif
}

static char *
local_getGridName(int rankInGroup, const char *base)
{
	char *name;
	int  charsToAlloc = strlen(base) + 1 + 1 + 3;

	name = xmalloc(sizeof(char) * charsToAlloc);
	sprintf(name, "%s_%03i", base, rankInGroup);

	return name;
}

static char **
local_getPatchNames(const char *base, int numPatches)
{
	char **names;
	int  charsToAlloc = strlen(base) + 1 + 3 + 1;

	names = xmalloc(sizeof(char *) * numPatches);
	for (int i = 0; i < numPatches; i++) {
		names[i] = xmalloc(sizeof(char) * charsToAlloc);
		sprintf(names[i], "%s_%03i", base, i);
	}

	return names;
}

static void
local_delPatchNames(char **patchNames, int numPatches)
{
	for (int i = 0; i < numPatches; i++)
		xfree(patchNames[i]);
	xfree(patchNames);
}

static void
local_writePatchData(gridWriterSilo_t writer,
                     gridPatch_t      patch,
                     const char       *patchName)
{
	int            numVars  = gridPatch_getNumVars(patch);
	char           *varName = NULL;
	gridPointInt_t dims;


	for (int i = 0; i < numVars; i++) {
		dataVar_t var          = gridPatch_getVarHandle(patch, i);
		void      *data        = gridPatch_getVarDataHandle(patch, i);
		int       varType      = local_getVarType(var);
		int       varCentering = DB_ZONECENT;

		for (int j = 0; j < NDIM; j++) {
			dims[j] = (int)(gridPatch_getOneDim(patch, j));
			if (varCentering == DB_NODECENT)
				dims[j]++;
		}

		varName = local_getPatchVarName(var, patchName, varName);
		DBPutQuadvar1(writer->f, varName, patchName, data, dims, NDIM,
		              NULL, 0, varType, varCentering, NULL);
	}

	if (varName != NULL)
		xfree(varName);
}

inline static int
local_getVarType(dataVar_t var)
{
	int varType;

	switch (dataVar_getType(var)) {
	case DATAVARTYPE_INT:
		varType = DB_INT;
		break;
	case DATAVARTYPE_INT8:
		varType = DB_CHAR;
		break;
	case DATAVARTYPE_DOUBLE:
		varType = DB_DOUBLE;
		break;
	case DATAVARTYPE_FPV:
		varType = (sizeof(fpv_t) == 4 ? DB_FLOAT : DB_DOUBLE);
		break;
	default:
		// We should never ever end up here.
		diediedie(EXIT_FAILURE);
	}

	return varType;
}

static void *
local_getPatchVarName(dataVar_t var, const char *patchName, char *varName)
{
	char *baseVarName = dataVar_getName(var);
	int  lenBaseName  = strlen(baseVarName);
	int  lenSuffix    = strlen(patchName);

	varName = xrealloc(varName, lenBaseName + lenSuffix + 1 + 1);
	sprintf(varName, "%s_%s", baseVarName, patchName);

	return varName;
}

static bool
local_dirExistsInFile(DBfile *db, const char *dname)
{
	bool  dirExists = false;
	DBtoc *toc      = DBGetToc(db);
	int   numDirs   = toc->ndir;

	for (int i = 0; i < numDirs; i++) {
		if (strcmp(toc->dir_names[i], dname) == 0) {
			dirExists = true;
			break;
		}
	}

	return dirExists ? true : false;
}

#endif
