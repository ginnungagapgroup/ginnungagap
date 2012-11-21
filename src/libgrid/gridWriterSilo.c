// Copyright (C) 2010, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libgrid/gridWriterSilo.c
 * @ingroup  libgridIOOutSilo
 * @brief  Implements the Silo writer.
 */


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriterSilo.h"
#include <assert.h>
#include <string.h>
#include <silo.h>
#ifdef WITH_MPI
#  include <mpi.h>
#  include <pmpio.h>
#endif
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridWriterSilo_adt.h"


/*--- Local defines -----------------------------------------------------*/

/** @brief  Number of digits used for the file numbers. */
#define LOCAL_NUMFILEDIGITS 3
#ifdef WITH_MPI
/** @brief  The prefix used for the directories in the files. */
#  define LOCAL_DIRPREFIX "domain_"
/** @brief  Number of digitis used for the directory numbers. */
#  define LOCAL_NUMDIRDIGITS 5
/** @brief  The MPI tag used in the PMPIO communication. */
#  define LOCAL_MPI_TAG 987
#endif

/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the functions table for the Silo writer. */
static struct gridWriter_func_struct local_func
    = {&gridWriterSilo_del,
	   &gridWriterSilo_activate,
	   &gridWriterSilo_deactivate,
	   &gridWriterSilo_writeGridPatch,
	   &gridWriterSilo_writeGridRegular,
#ifdef WITH_MPI
	   &gridWriterSilo_initParallel
#endif
	};

/** @brief  Gives the default path of the output file. */
static const char *local_defaultFileNamePath = NULL;

/** @brief  Gives the default prefix of the output file. */
static const char *local_defaultFileNamePrefix = "out";

/** @brief  Gives the default qualifier of the output file. */
static const char *local_defaultFileNameQualifier = NULL;

/** @brief  Gives the default suffix of the output file. */
static const char *local_defaultFileNameSuffix = ".silo";

/*--- Prototypes of local functions -------------------------------------*/

/**
 * @name  Opening and Closing Files (PMPIO support functions)
 *
 * @{
 */

/**
 * @brief  Creates a new DB file.
 *
 * This function is to be used with the PMPIO functionally of Silo, hence
 * the rather strange signature.  It is also used in the serial case, so be
 * sure to understand what this function does.
 *
 * Note that this function is called only by the first task in each PMPIO
 * group, all the other ones don't try to create a database but just open
 * the one that has been created by the first task in the group.
 *
 * Depending on the status of the writer, the function might not actually
 * try to create a file but open an existing one.  This will happen with
 * repeated gridWriter_activate()/gridWriter_deactivate() cycles.  Only the
 * very first call to gridWriter_activate() will trigger a call to creation
 * of a new db file, all other ones assume that the file has already been
 * created and can be appended to.
 *
 * @param[in]      *fname
 *                    The file name.  Must be a valid name, passing @c NULL
 *                    is undefined.
 * @param[in]      *dname
 *                    The directory that should be created and change to
 *                    within the file.  This may be either @c NULL or the
 *                    empty string, both signifying that no directory should
 *                    be created.
 * @param[in,out]  *udata
 *                    Additional user, effectively this must be the grid
 *                    silo writer the create function should work with.
 *                    Passing @c NULL is undefined.
 *
 * @return  Returns the Silo file pointer of the newly created file.
 */
static void *
local_createDB(const char *fname, const char *dname, void *udata);


#ifdef WITH_MPI

/**
 * @brief  Opens a database file for writing.
 *
 * This function is to be used with the PMPIO functionality of Silo and is
 * only used in the parallel version (the serial version always goes through
 * local_createDB() instead.
 *
 * @param[in]      *fname
 *                    The name of the file that should be opened.  Must be
 *                    valid file name, passing @c NULL is undefined.
 * @param[in]      *dname
 *                    The directory in the file that should be created for
 *                    the current process (and changed into).  This must be
 *                    a valid directory name, passing @c NULL is undefined.
 * @param[in]      iomode
 *                    The switches between reading and writing, currently
 *                    only writing makes sense, so this should not be
 *                    PMPIO_READ.
 * @param[in,out]  *udata
 *                    The user data, effectively this must be the silo
 *                    writer object.  Passing @c NULL is undefined.
 *
 * @return  Returns the Silo file pointer of the opened file.
 */
static void *
local_openDB(const char     *fname,
             const char     *dname,
             PMPIO_iomode_t iomode,
             void           *udata);

#endif

/**
 * @brief  Closes the data file.
 *
 * This is to be used within the PMPIO scheme, but can also in the serial
 * path for closing the database file.
 *
 * @param[in]      *file
 *                    The Silo file that should be closed.
 * @param[in,out]  *udata
 *                    The user data, effectively this must be the silo
 *                    writer object.  Closing the file will null the Silo
 *                    file pointer.  Passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_closeDB(void *file, void *udata);


/** @} */

/**
 * @name  Local Helper Functions
 *
 * @{
 */

/**
 * @brief  Sets the proper file name for this reader to use by adjusting the
 *         qualifier.
 *
 * @param[in,out]  writer
 *                    The writer to work with, passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_createFileName(gridWriterSilo_t writer);


/**
 * @brief  Creates the directory name for this writer (the domain).
 *
 * @param[in,out]  writer
 *                    The writer to work with, passing @c NULL is undefined.
 *
 * @return  Returns nothing.
 */
static void
local_createDirName(gridWriterSilo_t writer);


/**
 * @brief  Constructs a name for the grid by appending a number.
 *
 * @param[in]  rankInGroup
 *                The number that should be appended.
 * @param[in]  *base
 *                The string to which to append the number, must not be
 *                @c NULL, but may be the empty string.
 *
 * @return  Returns a new string containing  @c *base with @c rankInGroup
 *          zero-padded appended (separated by @c _).
 */
static char *
local_getGridName(int rankInGroup, const char *base);


/**
 * @brief  Constructs the patch names by taking the base and appending the
 *         patch numbers.
 *
 * @param[in]  *base
 *                The base name to which to append the patch numbers.  Must
 *                be a valid string.
 * @param[in]  numPatches
 *                The number of patches.
 *
 * @return  Returns an array of string, each holding the name for the
 *          according patch.
 */
static char **
local_getPatchNames(const char *base, int numPatches);


/**
 * @brief  Frees the patch names generated with local_getPatchNames().
 *
 * @param[in,out]  **patchNames
 *                    The name of patches as returned by
 *                    local_getPatchNames().
 * @param[in]      numPatches
 *                    The number of patches (must coincide with the number
 *                    used to generate the patch names).
 *
 * @return  Returns nothing.
 */
static void
local_delPatchNames(char **patchNames, int numPatches);


/**
 * @brief  Helper function to actually write the data at given patch.
 *
 * @param[in]  writer
 *                The writer to work with, passing @c NULL is undefined.
 * @param[in]  patch
 *                The patch that should be written.  Passing @c NULL is
 *                undefined.
 * @param[in]  *patchName
 *                The name that should be used for the patch.  Must be a
 *                valid string.
 *
 * @return  Returns nothing.
 */
static void
local_writePatchData(gridWriterSilo_t writer,
                     gridPatch_t      patch,
                     const char       *patchName);


/**
 * @brief  Translates the grid variable type to the corresponding Silo
 *         variable type.
 *
 * @param[in]  var
 *                The variable that should be translated.
 *
 * @return  Returns the corresponding Silo data type.
 */
inline static int
local_getVarType(dataVar_t var);


/**
 * @brief  Constructs the name for a variable at a given patch.
 *
 * @param[in]  var
 *                The variable that should be used.
 * @param[in]  *patchName
 *                The patch name.
 * @param[in]  *varName
 *                The previously used variable name that can be recycled for
 *                creating the new variable name.  May be @c NULL.
 *
 * @return  Returns the new variable name at the given patch.
 */
static char *
local_getPatchVarName(dataVar_t var, const char *patchName, char *varName);


/**
 * @brief  Checks if a given directory already exists in a file.
 *
 * @param[in]  *db
 *                The file in which to look for the existence of the
 *                directory.
 * @param[in]  *dname
 *                The directory to look for.
 *
 * @return  Returns @c true if the directory exists and @c false if not.
 */
static bool
local_dirExistsInFile(DBfile *db, const char *dname);


/** @} */

/*--- Implementations of abstract functions -----------------------------*/
extern void
gridWriterSilo_del(gridWriter_t *writer)
{
	assert(writer != NULL && *writer != NULL);
	assert((*writer)->type == GRIDIO_TYPE_SILO);

	gridWriter_free(*writer);
	gridWriterSilo_free((gridWriterSilo_t)*writer);

	xfree(*writer);
	*writer = NULL;
}

extern void
gridWriterSilo_activate(gridWriter_t writer)
{
	gridWriterSilo_t w = (gridWriterSilo_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_SILO);
#ifdef WITH_MPI
	assert(w->baton != NULL);
#endif

	if (!gridWriter_isActive(writer)) {
		local_createFileName(w);
		local_createDirName(w);
#ifdef WITH_MPI
		w->f = PMPIO_WaitForBaton(w->baton,
		                          filename_getFullName(w->base.fileName),
		                          w->dirName);
#else
		w->f = local_createDB(filename_getFullName(w->base.fileName),
		                      w->dirName, w);
#endif
		gridWriter_setIsActive(writer);
	}
}

extern void
gridWriterSilo_deactivate(gridWriter_t writer)
{
	gridWriterSilo_t w = (gridWriterSilo_t)writer;

	assert(w != NULL);
	assert(w->base.type == GRIDIO_TYPE_SILO);
#ifdef WITH_MPI
	assert(w->baton != NULL);
#endif

	if (gridWriter_isActive(writer)) {
#ifdef WITH_MPI
		PMPIO_HandOffBaton(w->baton, w->f);
#else
		local_closeDB(w->f, (void *)w);
#endif
		gridWriter_setIsInactive(writer);
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
	assert(tmp->base.type == GRIDIO_TYPE_SILO);
	assert(tmp->base.isActive);
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
	assert(tmp->base.type == GRIDIO_TYPE_SILO);
	assert(tmp->base.isActive);
	assert(grid != NULL);

#ifdef WITH_MPI
	gridName = local_getGridName(tmp->rankInGroup,
	                             gridRegular_getName(grid));
#else
	gridName   = local_getGridName(0, gridRegular_getName(grid));
#endif
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

#ifdef WITH_MPI
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

#endif

/*--- Implementations of final functions --------------------------------*/
extern gridWriterSilo_t
gridWriterSilo_new(void)
{
	gridWriterSilo_t writer;

	writer = gridWriterSilo_alloc();
	gridWriter_init((gridWriter_t)writer, GRIDIO_TYPE_SILO, &local_func);
	gridWriterSilo_init(writer);

	return writer;
}

extern void
gridWriterSilo_setDbType(gridWriterSilo_t w, int dbType)
{
	assert(w != NULL);
#ifdef WITH_HDF5
	assert(dbType == DB_PDB || dbType == DB_HDF5);
#else
	assert(dbType == DB_PDB);
#endif

	w->dbType = dbType;
}

extern void
gridWriterSilo_setNumFiles(gridWriterSilo_t w, int numFiles)
{
	assert(w != NULL);
	assert(numFiles > 0);
	assert(numFiles < (int)(pow(10., LOCAL_NUMFILEDIGITS)));

	w->numFiles = numFiles;
}

/*--- Implementations of protected functions ----------------------------*/
extern gridWriterSilo_t
gridWriterSilo_alloc(void)
{
	gridWriterSilo_t writer;

	writer = xmalloc(sizeof(struct gridWriterSilo_struct));

	return writer;
}

extern void
gridWriterSilo_init(gridWriterSilo_t writer)
{
	gridWriter_setFileName((gridWriter_t)writer,
	                       filename_newFull(local_defaultFileNamePath,
	                                        local_defaultFileNamePrefix,
	                                        local_defaultFileNameQualifier,
	                                        local_defaultFileNameSuffix));
#ifdef WITH_HDF5
	writer->dbType      = DB_HDF5;
#else
	writer->dbType      = DB_PDB;
#endif
	writer->numFiles    = 1;
	writer->f           = NULL;
	writer->dirName     = NULL;
#ifdef WITH_MPI
	writer->baton       = NULL;
	writer->groupRank   = -1;
	writer->rankInGroup = -1;
	writer->globalRank  = -1;
#endif
}

extern void
gridWriterSilo_free(gridWriterSilo_t w)
{
	assert(w != NULL);
	assert(w->f == NULL);

	if (w->dirName != NULL)
		xfree(w->dirName);
#ifdef WITH_MPI
	if (w->baton != NULL)
		PMPIO_Finish(w->baton);
#endif
}

/*--- Implementations of local functions --------------------------------*/
static void *
local_createDB(const char *fname, const char *dname, void *udata)
{
	gridWriterSilo_t writer = (gridWriterSilo_t)udata;

	if (!gridWriter_hasBeenActivated((gridWriter_t)writer)) {
		int mode = DB_NOCLOBBER;
		if (gridWriter_getOverwriteFileIfExists((gridWriter_t)writer))
			mode = DB_CLOBBER;
		writer->f = DBCreate(fname, mode, DB_LOCAL, NULL, writer->dbType);
	} else {
		writer->f = DBOpen(fname, writer->dbType, DB_APPEND);
	}
	if ((dname != NULL) && (dname[0] != '\0')) {
		if (!local_dirExistsInFile(writer->f, dname))
			DBMkDir(writer->f, dname);
		DBSetDir(writer->f, dname);
	}

	return writer->f;
}

#ifdef WITH_MPI
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

#endif

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
	char myQualifier[LOCAL_NUMFILEDIGITS + 2]; // account for \0 and _
	char *fullQualifier;

#ifdef WITH_MPI
	sprintf(myQualifier, "_%0*i", LOCAL_NUMFILEDIGITS, writer->groupRank);
#else
	myQualifier[0] = '\0';
#endif

	fullQualifier = xstrmerge(filename_getQualifier(writer->base.fileName),
	                          myQualifier);

	filename_setQualifier(writer->base.fileName, fullQualifier);

	xfree(fullQualifier);
}

static void
local_createDirName(gridWriterSilo_t writer)
{
	size_t charsToAlloc = 0;

	if (writer->dirName != NULL)
		xfree(writer->dirName);

	charsToAlloc    = 1;
#ifdef WITH_MPI
	charsToAlloc   += strlen(LOCAL_DIRPREFIX) + LOCAL_NUMDIRDIGITS;
#endif
	writer->dirName = xmalloc(charsToAlloc * sizeof(char));
#ifdef WITH_MPI
	sprintf(writer->dirName, "%s%0*i",
	        LOCAL_DIRPREFIX, LOCAL_NUMDIRDIGITS, writer->globalRank);
#else
	writer->dirName[0] = '\0';
#endif
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

static char *
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
