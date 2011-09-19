// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadget.c
 * @ingroup libutilFilesGadget
 * @brief  This file provides the implementations the Gadget object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadget.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"
#include "../libutil/xstring.h"
#include "gadgetUtil.h"
#include "gadgetHeader.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gadget_adt.h"


/*--- Local defines -----------------------------------------------------*/

/**
 * @brief  Gives the maximum amount of digits allowed in the file
 *         numbers.
 */
#define LOCAL_MAX_FILEDIGITS 7

/**
 * @brief  Gives the largest amount of physical files allowed.
 */
#define LOCAL_MAX_NUMFILES 10000000


/*--- Prototypes of local functions -------------------------------------*/
static void
local_constructFileNames(gadget_t gadget, const char *fileNameStem);


/*--- Implementations of exported functios ------------------------------*/
extern gadget_t
gadget_new(const char *fileNameStem, int numFiles)
{
	gadget_t gadget;

	assert(fileNameStem != NULL);
	assert(numFiles > 0 && numFiles < LOCAL_MAX_NUMFILES);

	gadget              = xmalloc(sizeof(struct gadget_struct));
	gadget->numFiles    = numFiles;
	gadget->fileVersion = 2;
	gadget->fileNames   = xmalloc(sizeof(char *) * numFiles);
	gadget->f           = xmalloc(sizeof(FILE *) * numFiles);
	gadget->headers     = xmalloc(sizeof(gadgetHeader_t) * numFiles);
	local_constructFileNames(gadget, fileNameStem);
	for (int i = 0; i < gadget->numFiles; i++) {
		gadget->f[i]       = NULL;
		gadget->headers[i] = NULL;
	}
	gadget->mode = GADGET_MODE_READ;

	return gadget;
}

extern void
gadget_del(gadget_t *gadget)
{
	assert(gadget != NULL && *gadget != NULL);

	for (int i = 0; i < (*gadget)->numFiles; i++) {
		xfree((*gadget)->fileNames[i]);
		if ((*gadget)->f[i] != NULL)
			xfclose(&((*gadget)->f[i]));
		if ((*gadget)->headers[i] != NULL)
			gadgetHeader_del(&((*gadget)->headers[i]));
	}
	xfree((*gadget)->fileNames);
	xfree((*gadget)->f);
	xfree((*gadget)->headers);

	xfree(*gadget);

	*gadget = NULL;
}

extern void
gadget_setFileVersion(gadget_t gadget, int version)
{
	assert(gadget != NULL);
	assert(version == 1 || version == 2);

	gadget->fileVersion = version;
}

extern int
gadget_getFileVersion(const gadget_t gadget)
{
	assert(gadget != NULL);

	return gadget->fileVersion;
}

extern int
gadget_getNumFiles(const gadget_t gadget)
{
	assert(gadget != NULL);

	return gadget->numFiles;
}

extern void
gadget_attachHeader(gadget_t gadget, gadgetHeader_t header, int numFile)
{
	assert(gadget != NULL);
	assert(header != NULL);
	assert(numFile >= 0 && numFile < gadget->numFiles);

	if (gadget->headers[numFile] != NULL)
		gadgetHeader_del(&(gadget->headers[numFile]));

	gadget->headers[numFile] = header;
}

extern void
gadget_open(gadget_t gadget, gadgetMode_t mode, int numFile)
{
	assert(gadget != NULL);
	assert(numFile >= 0 && numFile < gadget->numFiles);

	if (gadget->f[numFile] != NULL)
		xfclose(&(gadget->f[numFile]));

	gadget->f[numFile] = xfopen(gadget->fileNames[numFile],
	                            mode == GADGET_MODE_READ ? "rb" : "wb");
	gadget->mode       = mode;
}

extern void
gadget_close(gadget_t gadget, int numFile)
{
	assert(gadget != NULL);
	assert(numFile >= 0 && numFile < gadget->numFiles);

	if (gadget->f[numFile] != NULL)
		xfclose(&(gadget->f[numFile]));
}

extern void
gadget_write(gadget_t                 gadget,
             int                      numFile,
             const float *restrict    pos,
             const float *restrict    vel,
             const uint32_t *restrict id)
{
	uint32_t block;
	uint32_t numPartWrite;

	assert(gadget != NULL);
	assert(numFile >= 0 && numFile < gadget->numFiles);
	assert(gadget->f[numFile] != NULL);
	assert(gadget->mode == GADGET_MODE_WRITE);
	assert(gadget->headers[numFile] != NULL);

	numPartWrite = gadgetHeader_getNumPartsInFile(gadget->headers[numFile]);

	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "HEAD",
		                      GADGETHEADER_SIZE);
	gadgetHeader_write(gadget->headers[numFile], gadget->f[numFile]);

	block = numPartWrite * 3 * sizeof(float);
	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "POS ", block);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
	xfwrite(pos, sizeof(float) * 3, numPartWrite, gadget->f[numFile]);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);

	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "VEL ", block);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
	xfwrite(vel, sizeof(float) * 3, numPartWrite, gadget->f[numFile]);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);

	block = numPartWrite * sizeof(uint32_t);
	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "ID  ", block);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
	xfwrite(id, sizeof(uint32_t), numPartWrite, gadget->f[numFile]);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
} /* gadget_write */

extern void
gadget_writeLong(gadget_t                 gadget,
                 int                      numFile,
                 const float *restrict    pos,
                 const float *restrict    vel,
                 const uint64_t *restrict id)
{
	uint32_t block;
	uint32_t numPartWrite;

	assert(gadget != NULL);
	assert(numFile >= 0 && numFile < gadget->numFiles);
	assert(gadget->f[numFile] != NULL);
	assert(gadget->mode == GADGET_MODE_WRITE);
	assert(gadget->headers[numFile] != NULL);

	numPartWrite = gadgetHeader_getNumPartsInFile(gadget->headers[numFile]);

	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "HEAD",
		                      GADGETHEADER_SIZE);
	gadgetHeader_write(gadget->headers[numFile], gadget->f[numFile]);

	block = numPartWrite * 3 * sizeof(float);
	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "POS ", block);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
	xfwrite(pos, sizeof(float) * 3, numPartWrite, gadget->f[numFile]);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);

	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "VEL ", block);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
	xfwrite(vel, sizeof(float) * 3, numPartWrite, gadget->f[numFile]);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);

	block = numPartWrite * sizeof(uint64_t);
	if (gadget->fileVersion >= 2)
		gadgetUtil_writeBlock(gadget->f[numFile], "ID  ", block);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
	xfwrite(id, sizeof(uint64_t), numPartWrite, gadget->f[numFile]);
	xfwrite(&block, sizeof(uint32_t), 1, gadget->f[numFile]);
} /* gadget_writeLong */

/*--- Implementations of local functions --------------------------------*/
static void
local_constructFileNames(gadget_t gadget, const char *fileNameStem)
{
	char number[LOCAL_MAX_FILEDIGITS + 1];

	if (gadget->numFiles > 1) {
		for (int i = 0; i < gadget->numFiles; i++) {
			sprintf(number, ".%i", i);
			gadget->fileNames[i] = xstrmerge(fileNameStem, number);
		}
	} else {
		gadget->fileNames[0] = xstrdup(fileNameStem);
	}
}
