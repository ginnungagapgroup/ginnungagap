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
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xstring.h"
#include "../../src/libutil/varArr.h"
#include "siloRoot.h"


/*--- Implemention of main structure ------------------------------------*/
#include "makeSiloRoot_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
static void
local_checkOutputName(const char *outName, bool force);

static int
local_getFileNames(const char *stem, const char *suffix, char ***target);

static bool
local_testForSingleFile(const char *stem, const char *suffix, char *tmp);

static int
local_getNumFiles(const char *stem, const char *suffix, char *tmp);

static void
local_getTestName(const char *stem,
                  const char *suffix,
                  int        numFiles,
                  char       *testName);


/*--- Implementations of exported functios ------------------------------*/
extern makeSiloRoot_t
makeSiloRoot_new(const char *siloFileStem,
                 const char *siloFileSuffix,
                 const char *siloOutputFileName,
                 bool       force)
{
	makeSiloRoot_t msr;

	msr           = xmalloc(sizeof(struct makeSiloRoot_struct));
	local_checkOutputName(siloOutputFileName, force);
	msr->siloRoot = siloRoot_new(siloOutputFileName);
	msr->numFiles = local_getFileNames(siloFileStem,
	                                   siloFileSuffix,
	                                   &(msr->fileNames));

	return msr;
}

extern void
makeSiloRoot_run(makeSiloRoot_t msr)
{
	assert(msr != NULL);

	for (int i = 0; i < msr->numFiles; i++)
		siloRoot_parseFile(msr->siloRoot, msr->fileNames[i]);
	siloRoot_write(msr->siloRoot);
}

extern void
makeSiloRoot_del(makeSiloRoot_t *msr)
{
	assert(msr != NULL);
	assert(*msr != NULL);

	for (int i = 0; i < (*msr)->numFiles; i++) {
		xfree((*msr)->fileNames[i]);
	}
	xfree((*msr)->fileNames);
	siloRoot_del(&((*msr)->siloRoot));
	xfree(*msr);
	*msr = NULL;
}

/*--- Implementations of local functions --------------------------------*/
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
