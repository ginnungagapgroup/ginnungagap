// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "makeSiloRootConfig.h"
#include "makeSiloRoot.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include <stdbool.h>
#include <silo.h>
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/varArr.h"


/*--- Implemention of main structure ------------------------------------*/
#include "makeSiloRoot_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
static int
local_getFileNames(const char *stem, const char *suffix, char ***target);

static bool
local_testForSingleFile(const char *stem, const char *suffix, char *tmp);

static void
local_getTestName(const char *stem,
                  const char *suffix,
                  int        numFiles,
                  char       *testName);

static int
local_getNumFiles(const char *stem, const char *suffix, char *tmp);

static void
local_checkOutputName(const char *outName, bool force);

static void
local_adjustCurrentPath(char       **path,
                        const char *dirName,
                        const char *fileName);

static void
local_addToMeshNames(varArr_t   meshes,
                     const char *meshName,
                     const char *path);

static void
local_output(makeSiloRoot_t msr, varArr_t meshNames);


/*--- Implementations of exported functios ------------------------------*/
extern makeSiloRoot_t
makeSiloRoot_new(const char *siloFileStem,
                 const char *siloFileSuffix,
                 const char *siloOutputFileName,
                 bool       force)
{
	makeSiloRoot_t msr;

	msr              = xmalloc(sizeof(struct makeSiloRoot_struct));
	local_checkOutputName(siloOutputFileName, force);
	msr->outFileName = xstrdup(siloOutputFileName);
	msr->numFiles    = local_getFileNames(siloFileStem,
	                                      siloFileSuffix,
	                                      &(msr->fileNames));

	return msr;
}

extern void
makeSiloRoot_run(makeSiloRoot_t msr)
{
	char     *path     = NULL;
	varArr_t meshNames = NULL;
	DBfile   *file     = NULL;
	DBtoc    *toc      = NULL;

	assert(msr != NULL);

	meshNames = varArr_new(1);

	fprintf(stdout, "\n");
	for (int i = 0; i < msr->numFiles; i++) {
		fprintf(stdout, "Checking %s\n", msr->fileNames[i]);
		file = DBOpen(msr->fileNames[i], DB_HDF5, DB_READ);
		if (file == NULL)
			exit(EXIT_FAILURE);
		toc = DBGetToc(file);
		for (int j = 0; j < toc->ndir; j++) {
			local_adjustCurrentPath(&path,
			                        toc->dir_names[j],
			                        msr->fileNames[i]);
			DBSetDir(file, toc->dir_names[j]);
			toc = DBGetToc(file);
			for (int k = 0; k < toc->nqmesh; k++)
				local_addToMeshNames(meshNames, toc->qmesh_names[k], path);
			DBSetDir(file, "/");
			toc = DBGetToc(file);
		}
		DBClose(file);
	}

	local_output(msr, meshNames);

	varArr_del(&meshNames);
} /* makeSiloRoot_run */

extern void
makeSiloRoot_del(makeSiloRoot_t *msr)
{
	assert(msr != NULL);
	assert(*msr != NULL);

	for (int i = 0; i < (*msr)->numFiles; i++) {
		xfree((*msr)->fileNames[i]);
	}
	xfree((*msr)->fileNames);
	xfree((*msr)->outFileName);
	xfree(*msr);
	*msr = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static int
local_getFileNames(const char *stem, const char *suffix, char ***target)
{
	char *tmp;
	int  numFiles = 0;
	int  bytesToAlloc;
	bool single;

	assert(stem != NULL);

	bytesToAlloc = strlen(stem) + 3 + 1 + strlen(suffix);
	tmp          = xmalloc(sizeof(char) * bytesToAlloc);

	fprintf(stdout, "\nLooking for files\n");
	if (!(single = local_testForSingleFile(stem, suffix, tmp)))
		numFiles = local_getNumFiles(stem, suffix, tmp);
	else
		numFiles = 1;

	*target = xmalloc(sizeof(char *) * numFiles);
	if (!single) {
		for (int i = 0; i < numFiles; i++) {
			local_getTestName(stem, suffix, i, tmp);
			(*target)[i] = xstrdup(tmp);
		}
	} else {
		(*target)[0] = xstrdup(tmp);
	}

	xfree(tmp);

	return numFiles;
}

static bool
local_testForSingleFile(const char *stem, const char *suffix, char *tmp)
{
	FILE *f;

	sprintf(tmp, "%s%s", stem, suffix);
	f = fopen(tmp, "r");
	if (f != NULL) {
		fclose(f);
		fprintf(stdout, "\t%s found\n", tmp);
		return true;
	}
	return false;
}

static int
local_getNumFiles(const char *stem, const char *suffix, char *tmp)
{
	FILE *f;
	int  numFiles = 0;

	local_getTestName(stem, suffix, numFiles, tmp);
	while ((f = fopen(tmp, "r")) != NULL) {
		fprintf(stdout, "\t%s found\n", tmp);
		fclose(f);
		numFiles++;
		local_getTestName(stem, suffix, numFiles, tmp);
	}

	if (numFiles == 0) {
		fprintf(stderr,
		        "No files found that match %s([0-9][0-9][0-9])%s :-(\n",
		        stem, (suffix != NULL ? suffix : ""));
		exit(EXIT_FAILURE);
	}

	return numFiles;
}

static void
local_getTestName(const char *stem,
                  const char *suffix,
                  int        numFiles,
                  char       *testName)
{
	sprintf(testName, "%s%03i%s", stem, numFiles, suffix);
}

static void
local_checkOutputName(const char *outName, bool force)
{
	FILE *f;

	f = fopen(outName, "r");

	if (f != NULL) {
		if (!force) {
			fprintf(stderr,
			        "Output file %s already exists.  "
			        "Use --force to overwrite.\n",
			        outName);
			exit(EXIT_FAILURE);
		}
		fclose(f);
		fprintf(stdout,
		        "Warning:  Output file %s already will be overwritten.\n",
		        outName);
	}
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
local_addToMeshNames(varArr_t   meshes,
                     const char *meshName,
                     const char *path)
{
	char *completeName = NULL;
	int  bytesNeeded   = strlen(path) + strlen(meshName) + 1;

	completeName = xmalloc(sizeof(char) * bytesNeeded);
	sprintf(completeName, "%s%s", path, meshName);

	(void)varArr_insert(meshes, completeName);
}

static void
local_output(makeSiloRoot_t msr, varArr_t meshNames)
{
	DBfile *file = NULL;
	char **strList;
	int *types;
	int nMesh;

	file = DBCreate(msr->outFileName, DB_CLOBBER, DB_LOCAL, NULL, DB_HDF5);

	nMesh = varArr_getLength(meshNames);
	strList = xmalloc(sizeof(char *) * nMesh);
	types = xmalloc(sizeof(int) * nMesh);
	for (int i=0 ;i<nMesh; i++) {
		strList[i] = varArr_remove(meshNames, 0);
		types[i] = DB_QUAD_RECT;
	}

	DBPutMultimesh(file, "mesh", nMesh, strList, types, NULL);

	for (int i=0; i<nMesh; i++)
		xfree(strList[i]);
	xfree(types);
	xfree(strList);

	DBClose(file);
}
