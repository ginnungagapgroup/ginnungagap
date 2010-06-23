// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "makeSiloRootConfig.h"
#include "siloRoot.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <silo.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "siloRoot_adt.h"


/*--- Local defines -----------------------------------------------------*/
#define CHECK_NULL(ptr)                                     \
	{                                                       \
		if ((ptr) == NULL) {                                \
			fprintf(stderr, "Null pointer at %s:%i (%s)\n", \
			        __FILE__, __LINE__, __func__);          \
			exit(EXIT_FAILURE);                             \
		}                                                   \
	}


/*--- Prototypes of local functions -------------------------------------*/
static void
local_writeMesh(DBfile *file, const char *name, varArr_t meshNames);

static void
local_writeVar(DBfile     *file,
               const char *name,
               varArr_t   varNames,
               char       *meshName);

static void
local_init(siloRoot_t siloRoot, const char *fileName);

static char *
local_getName(const char *longName);

static void
local_adjustCurrentPath(char       **path,
                        const char *dirName,
                        const char *fileName);

static void
local_parseDir(siloRoot_t sr, const char *path, DBfile *file);

static void
local_addToNames(varArr_t   names,
                 const char *newName,
                 const char *path);

static int
local_findVar(char **varMasterNames, const char *varName, int numVars);


/*--- Implementations of exported functios ------------------------------*/
extern siloRoot_t
siloRoot_new(const char *outFileName)
{
	siloRoot_t siloRoot;

	assert(outFileName != NULL);

	siloRoot                 = xmalloc(sizeof(struct siloRoot_struct));
	siloRoot->outFileName    = xstrdup(outFileName);
	siloRoot->meshMasterName = NULL;
	siloRoot->meshNames      = NULL;
	siloRoot->numVars        = 0;
	siloRoot->varMasterNames = 0;
	siloRoot->varNames       = NULL;
	siloRoot->initialized    = false;
	siloRoot->logOut         = stdout;

	return siloRoot;
}

extern void
siloRoot_del(siloRoot_t *siloRoot)
{
	assert(siloRoot != NULL && *siloRoot != NULL);

#define sr (*siloRoot)
	if (sr->initialized) {
		xfree(sr->outFileName);
		xfree(sr->meshMasterName);
		while (varArr_getLength(sr->meshNames) > 0)
			xfree(varArr_remove(sr->meshNames, 0));
		varArr_del(&(sr->meshNames));
		for (int i = 0; i < sr->numVars; i++) {
			xfree(sr->varMasterNames[i]);
			while (varArr_getLength(sr->varNames[i]) > 0)
				xfree(varArr_remove(sr->varNames[i], 0));
			varArr_del(&(sr->varNames[i]));
		}
		xfree(sr->varMasterNames);
		xfree(sr->varNames);
	}
#undef sr
	xfree(*siloRoot);

	*siloRoot = NULL;
}

extern void
siloRoot_write(siloRoot_t siloRoot)
{
	DBfile *file = NULL;

	assert(siloRoot != NULL);
	assert(siloRoot->initialized);

	file = DBCreate(siloRoot->outFileName, DB_CLOBBER,
	                DB_LOCAL, NULL, DB_HDF5);

	local_writeMesh(file, siloRoot->meshMasterName, siloRoot->meshNames);
	fprintf(siloRoot->logOut,
	        "Wrote multimesh %s (consisting of %i meshes)\n",
	        siloRoot->meshMasterName,
	        varArr_getLength(siloRoot->meshNames));
	for (int i = 0; i < siloRoot->numVars; i++) {
		local_writeVar(file, siloRoot->varMasterNames[i],
		               siloRoot->varNames[i], siloRoot->meshMasterName);
		fprintf(siloRoot->logOut,
		        "Wrote multivar %s (consisting of %i vars)\n",
		        siloRoot->varMasterNames[i],
		        varArr_getLength(siloRoot->varNames[i]));
	}

	DBClose(file);
}

extern void
siloRoot_parseFile(siloRoot_t siloRoot, const char *siloFileName)
{
	DBfile *file   = NULL;
	DBtoc  *toc    = NULL;
	char   *path   = NULL;
	int    numDirs = 0;

	if (!siloRoot->initialized) {
		local_init(siloRoot, siloFileName);
	}

	file    = DBOpen(siloFileName, DB_HDF5, DB_READ);
	CHECK_NULL(file);
	toc     = DBGetToc(file);
	CHECK_NULL(toc);
	numDirs = toc->ndir;

	for (int i = 0; i < numDirs; i++) {
		local_adjustCurrentPath(&path, toc->dir_names[i], siloFileName);
		DBSetDir(file, toc->dir_names[i]);
		local_parseDir(siloRoot, path, file);
		DBSetDir(file, "/");
		toc = DBGetToc(file);
	}

	DBClose(file);
	if (path != NULL)
		xfree(path);
}

/*--- Implementations of local functions --------------------------------*/
static void
local_writeMesh(DBfile *file, const char *name, varArr_t meshNames)
{
	char **strList;
	int  *types;
	int  nMesh;

	nMesh   = varArr_getLength(meshNames);
	strList = xmalloc(sizeof(char *) * nMesh);
	types   = xmalloc(sizeof(int) * nMesh);
	for (int i = 0; i < nMesh; i++) {
		strList[i] = varArr_getElementHandle(meshNames, i);
		types[i]   = DB_QUAD_RECT;
	}

	DBPutMultimesh(file, name, nMesh, strList, types, NULL);

	xfree(types);
	xfree(strList);
}

static void
local_writeVar(DBfile     *file,
               const char *name,
               varArr_t   varNames,
               char       *meshName)
{
	char      **strList;
	int       *types;
	int       nVars;
	DBoptlist *optList = NULL;

	nVars   = varArr_getLength(varNames);
	strList = xmalloc(sizeof(char *) * nVars);
	types   = xmalloc(sizeof(int) * nVars);
	for (int i = 0; i < nVars; i++) {
		strList[i] = varArr_getElementHandle(varNames, i);
		types[i]   = DB_QUADVAR;
	}
	optList = DBMakeOptlist(1);
	DBAddOption(optList, DBOPT_MMESH_NAME, meshName);
	DBPutMultivar(file, name, nVars, strList, types, optList);

	DBFreeOptlist(optList);
	xfree(types);
	xfree(strList);
}

static void
local_init(siloRoot_t sr, const char *fileName)
{
	DBfile *file;
	DBtoc  *toc;

	file = DBOpen(fileName, DB_HDF5, DB_READ);
	CHECK_NULL(file);
	toc  = DBGetToc(file);
	CHECK_NULL(toc);

	if (toc->ndir == 0) {
		fprintf(stderr, "No directory found in %s :(\n", fileName);
		exit(EXIT_FAILURE);
	}

	DBSetDir(file, toc->dir_names[0]);
	toc = DBGetToc(file);
	CHECK_NULL(toc);
	if (toc->nqmesh != 1) {
		fprintf(stderr, "Did not find a unique quad mesh in %s :(\n",
		        fileName);
		exit(EXIT_FAILURE);
	}
	sr->meshMasterName = local_getName(toc->qmesh_names[0]);
	sr->meshNames      = varArr_new(32);
	sr->numVars        = toc->nqvar;
	if (sr->numVars > 0) {
		sr->varMasterNames = xmalloc(sizeof(char *) * sr->numVars);
		sr->varNames       = xmalloc(sizeof(varArr_t) * sr->numVars);
		for (int i = 0; i < sr->numVars; i++) {
			sr->varMasterNames[i] = local_getName(toc->qvar_names[i]);
			sr->varNames[i]       = varArr_new(32);
		}
	}

	DBClose(file);

	sr->initialized = true;
} /* local_init */

static char *
local_getName(const char *longName)
{
	int  i     = 0;
	int  len   = strlen(longName);
	char *name = NULL;

	while (isalnum(longName[i]) && i < len)
		i++;
	name      = xmalloc(sizeof(char) * (i + 1));
	name[i--] = '\0';
	while (i >= 0) {
		name[i] = longName[i];
		i--;
	}

	return name;
}

static void
local_adjustCurrentPath(char       **path,
                        const char *dirName,
                        const char *fileName)
{
	int bytesNeeded = strlen(dirName) + strlen(fileName) + 3;
	int bytesAvail  = 0;

	if (*path != NULL)
		bytesAvail = strlen(*path) + 1;

	if (bytesNeeded > bytesAvail)
		*path = xrealloc(*path, sizeof(char) * bytesNeeded);

	sprintf(*path, "%s:%s/", fileName, dirName);
}

static void
local_parseDir(siloRoot_t sr, const char *path, DBfile *file)
{
	DBtoc *toc;

	toc = DBGetToc(file);
	if (toc->nqmesh != 1) {
		fprintf(stderr, "Did not find a unique quad mesh in %s :(\n", path);
		exit(EXIT_FAILURE);
	}
	if (toc->nqvar != sr->numVars) {
		fprintf(stderr, "Did not find %i vars in %s :(\n",
		        sr->numVars, path);
		exit(EXIT_FAILURE);
	}

	local_addToNames(sr->meshNames, toc->qmesh_names[0], path);
	for (int i = 0; i < toc->nqvar; i++) {
		int varPos = local_findVar(sr->varMasterNames,
		                           toc->qvar_names[i], sr->numVars);
		local_addToNames(sr->varNames[varPos], toc->qvar_names[i], path);
	}
}

static void
local_addToNames(varArr_t   names,
                 const char *newName,
                 const char *path)
{
	char *completeName = NULL;
	int  bytesNeeded   = strlen(path) + strlen(newName) + 1;

	completeName = xmalloc(sizeof(char) * bytesNeeded);
	sprintf(completeName, "%s%s", path, newName);

	(void)varArr_insert(names, completeName);
}

static int
local_findVar(char **varMasterNames, const char *varName, int numVars)
{
	bool found = false;
	int  i     = 0;
	int  len0  = strlen(varMasterNames[i]);

	while (!found && i < numVars) {
		char *s1  = local_getName(varName);
		int  len1 = strlen(s1);

		xfree(s1);

		if ((len0 == len1)
		    && (strncmp(varMasterNames[i], varName, len0) == 0))
			found = true;
		else
			i++;
	}
	if (!found) {
		fprintf(stderr, "Could not find %s\n", varName);
		exit(EXIT_FAILURE);
	}

	return i;
}
