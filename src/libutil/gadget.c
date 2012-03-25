// Copyright (C) 2010, 2011, 2012, Steffen Knollmann
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
#include "../libutil/endian.h"
#include "../libutil/xfile.h"
#include "../libutil/xstring.h"
#include "../libutil/byteswap.h"
#include "../libutil/diediedie.h"
#include "gadgetVersion.h"
#include "gadgetBlock.h"
#include "gadgetHeader.h"
#include "gadgetTOC.h"


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

/**
 * @brief  Gives the largest size a single component of an element in the
 *         file can have.
 */
#define LOCAL_MAX_SIZE_COMPONENT_IN_BYTES 16


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Will decrease the size of the ADT arrays to reflect a new
 *         choice for the number of files.
 *
 * This will also deal with freeing the memory used by the disposed ADT
 * array elements.  All other elements will be left untouched.  It can
 * be used to completely free the ADT arrays, if @c numFiles is 0.
 * After completion, the new number of files will be set.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object to work with.
 * @param[in]      numFiles
 *                    The new number of files.  This must be smaller
 *                    than the current number of files, but may not be
 *                    negative.  If 0, the ADT arrays will be freed.
 *
 * @return  Returns nothing.
 */
static void
local_shrinkADTArrays(gadget_t gadget, int numFiles);


/**
 * @brief  Will enlarge the ADT arrays to reflect a new choice for the
 *         number of files.
 *
 * This will also deal with creating the ADT arrays (if the current
 * number of files is 0) and also initialize the new elements to safe
 * values.
 *
 * @param[in,out]  gadget
 *                    The Gadget file object to work with.
 * @param[in]      numFiles
 *                    The new number of files.  This must be larger
 *                    than the current number of files.
 *
 * @return  Returns nothing.
 */
static void
local_enlargeADTArrays(gadget_t gadget, int numFiles);


/**
 * @brief  This will seek to a given position in the block.
 *
 * @param[in,out]  *f
 *                    The file pointer that should be put to the correct
 *                    position.  This must point to the beginning of the
 *                    data block (to the leading block size integer).
 * @param[in]      pSkip
 *                    The number of particles that should be skipped.
 * @param[in]      sizeOfElement
 *                    The size of each element in bytes.
 *
 * @return  Returns nothing.
 */
inline static void
local_skipToReqPosInBlock(FILE *f, uint64_t pSkip, size_t sizeOfElement);


/**
 * @brief  This is the actual function that deals with writing a block.
 *
 * @param[in,out]  *f
 *                    The file pointer.  Needs to point to the actual
 *                    position in file at which to start writing to.
 * @param[in]      *stai
 *                    The abstract data description.
 * @param[in]      doByteSwap
 *                    Toggles if the data needs to be adjusted for
 *                    endianess.
 * @param[in]      pWrite
 *                    The number of particle to write.
 * @param[in]      sizeOfElementFile
 *                    The size of one element in the file.
 * @param[in]      numComponents
 *                    The number of components of each element.
 * @param[in]      isInteger
 *                    Toggles whether the data is integer or floating point.
 *                    This is required to properly down- or up-cast the
 *                    data.
 *
 * @return  Returns nothing.
 */
inline static void
local_writeBlockActual(FILE         *f,
                       const stai_t stai,
                       bool         doByteSwap,
                       uint32_t     pWrite,
                       size_t       sizeOfElementFile,
                       int          numComponents,
                       bool         isInteger);


/**
 * @brief  This is the general version of writing data to the file.
 *
 * This works element by element: Every element is retrieved and, if
 * required, adjusted for endianess and up- or down-cast to adjust with
 * different precisions in the file and in memory.
 *
 * @param[in,out]  *f
 *                    The file pointer.  Needs to point to the actual
 *                    position in file at which to start writing to.
 * @param[in]      pWrite
 *                    The number of particle to write.
 * @param[in]      doByteSwap
 *                    Toggles if the data needs to be adjusted for
 *                    endianess.
 * @param[in]      stai
 *                    The abstract data description.
 * @param[in]      sizeOfElement
 *                    The size of one element in the file.
 * @param[in]      sizeOfElementStai
 *                    The size of one element in the memory representation
 *                    of the data.
 * @param[in]      numComponents
 *                    The number of components of each element.
 * @param[in]      isInteger
 *                    Toggles whether the data is integer or floating point.
 *                    This is required to properly down- or up-cast the
 *                    data.
 *
 * @return  Returns nothing.
 */
static void
local_writeBlockActualGeneral(FILE         *f,
                              uint64_t     pWrite,
                              bool         doByteSwap,
                              const stai_t stai,
                              size_t       sizeOfElement,
                              size_t       sizeOfElementStai,
                              int          numComponents,
                              bool         isInteger);


/**
 * @brief  This is the actual function that deals with reading a block.
 *
 * @param[in,out]  *f
 *                    The file pointer.  Needs to point to the actual
 *                    position in file at which to start writing to.
 * @param[in]      stai
 *                    The abstract data description.
 * @param[in]      doByteSwap
 *                    Toggles if the data needs to be adjusted for
 *                    endianess.
 * @param[in]      pRead
 *                    The number of particle to read.
 * @param[in]      sizeOfElementFile
 *                    The size of one element in the file.
 * @param[in]      numComponents
 *                    The number of components of each element.
 * @param[in]      isInteger
 *                    Toggles whether the data is integer or floating point.
 *                    This is required to properly down- or up-cast the
 *                    data.
 *
 * @return  Returns nothing.
 */
inline static void
local_readBlockActual(FILE     *f,
                      stai_t   stai,
                      bool     doByteSwap,
                      uint64_t pRead,
                      size_t   sizeOfElementFile,
                      int      numComponents,
                      bool     isInteger);


/**
 * @brief  This is the general version of reading data from the file.
 *
 * This works element by element: Every element is retrieved and, if
 * required, adjusted for endianess and up- or down-cast to adjust with
 * different precisions in the file and in memory.
 *
 * @param[in,out]  *f
 *                    The file pointer.  Needs to point to the actual
 *                    position in file at which to start reading from.
 * @param[in]      pRead
 *                    The number of particle to read.
 * @param[in]      doByteSwap
 *                    Toggles if the data needs to be adjusted for
 *                    endianess.
 * @param[in]      stai
 *                    The abstract data description.
 * @param[in]      sizeOfElement
 *                    The size of one element in the file.
 * @param[in]      sizeOfElementStai
 *                    The size of one element in the memory representation
 *                    of the data.
 * @param[in]      numComponents
 *                    The number of components of each element.
 * @param[in]      isInteger
 *                    Toggles whether the data is integer or floating point.
 *                    This is required to properly down- or up-cast the
 *                    data.
 *
 * @return  Returns nothing.
 */
static void
local_readBlockActualGeneral(FILE     *f,
                             uint64_t pRead,
                             bool     doByteSwap,
                             stai_t   stai,
                             size_t   sizeOfElement,
                             size_t   sizeOfElementStai,
                             int      numComponents,
                             bool     isInteger);


/**
 * @brief  Downcasts an element from 64bit to 32bit.
 *
 * @param[in]   eleHi
 *                 The 64bit element (may have multiple components).
 * @param[out]  eleLo
 *                 The 32bit element (may have multiple components).
 * @param[in]   numComponents
 *                 The number of components.
 * @param[in]   isInteger
 *                 Toggles between integer values and floating point values.
 *
 * @return  Returns nothing.
 */
inline static void
local_downcastElement(const void *restrict eleHi,
                      void *restrict       eleLo,
                      int                  numComponents,
                      bool                 isInteger);


/**
 * @brief  Upcasts an element from 32bit to 64bit.
 *
 * @param[in]   eleLo
 *                 The 32bit element (may have multiple components).
 * @param[out]  eleHi
 *                 The 64bit element (may have multiple components).
 * @param[in]   numComponents
 *                 The number of components.
 * @param[in]   isInteger
 *                 Toggles between integer values and floating point values.
 *
 * @return  Returns nothing.
 */
inline static void
local_upcastElement(const void *restrict eleLo,
                    void *restrict       eleHi,
                    int                  numComponents,
                    bool                 isInteger);


/*--- Implementations of exported functions -----------------------------*/
extern gadget_t
gadget_new(void)
{
	gadget_t gadget;

	gadget              = xmalloc(sizeof(struct gadget_struct));
	gadget->numFiles    = 0;
	gadget->fileVersion = 2;
	gadget->fileNames   = NULL;
	gadget->doByteSwap  = false;
	gadget->f           = NULL;
	gadget->mode        = GADGET_MODE_READ;
	gadget->lastOpened  = -1;
	gadget->headers     = NULL;
	gadget->tocs        = NULL;

	return gadget;
}

extern gadget_t
gadget_newSimple(const char *fileNameStem, int numFiles)
{
	gadget_t gadget;

	assert(fileNameStem != NULL);
	assert(numFiles > 0 && numFiles < LOCAL_MAX_NUMFILES);

	gadget = gadget_new();
	local_enlargeADTArrays(gadget, numFiles);

	gadget_setFileNamesFromStem(gadget, fileNameStem);

	return gadget;
}

extern void
gadget_del(gadget_t *gadget)
{
	assert(gadget != NULL && *gadget != NULL);

	if ((*gadget)->f != NULL)
		xfclose(&((*gadget)->f));

	if ((*gadget)->numFiles > 0)
		local_shrinkADTArrays(*gadget, 0);

	xfree(*gadget);

	*gadget = NULL;
}

/*--- Initialisation ----------------------------------------------------*/
extern void
gadget_initForRead(gadget_t gadget)
{
	assert(gadget != NULL);
	assert(gadget->fileNames != NULL);

	for (int i = 0; i < gadget->numFiles; i++) {
		gadget_open(gadget, GADGET_MODE_READ, i);
		gadget_detectByteswappingOfCurrentFile(gadget);
		gadget_detectFileVersionOfCurrentFile(gadget);
		gadget_readTOCFromCurrentFile(gadget);
		gadget_readHeaderFromCurrentFile(gadget);
		gadget_close(gadget);
	}
}

/*--- Setter ------------------------------------------------------------*/
extern void
gadget_setNumFiles(gadget_t gadget, int numFiles)
{
	assert(gadget != NULL);
	assert(numFiles > 0 && numFiles < LOCAL_MAX_NUMFILES);

	if (gadget->numFiles > 0) {
		if (numFiles < gadget->numFiles)
			local_shrinkADTArrays(gadget, numFiles);
		else if (numFiles > gadget->numFiles)
			local_enlargeADTArrays(gadget, numFiles);
		// Doing nothing if  numFiles == gadget->numFiles
	}
}

extern void
gadget_setFileVersion(gadget_t gadget, gadgetVersion_t version)
{
	assert(gadget != NULL);

	if (gadget->fileVersion != version) {
		gadget->fileVersion = version;
		for (int i = 0; i < gadget->numFiles; i++) {
			if (gadget->tocs[i] != NULL)
				gadgetTOC_setFileVersion(gadget->tocs[i], version);
		}
	}
}

extern void
gadget_setFileNameOfFile(gadget_t gadget, int fileNumber, char *fileName)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);

	if (gadget->fileNames[fileNumber] != NULL)
		xfree(gadget->fileNames[fileNumber]);
	gadget->fileNames[fileNumber] = fileName;
}

extern void
gadget_setFileNamesFromStem(gadget_t gadget, const char *stem)
{
	assert(gadget != NULL);
	assert(stem != NULL);

	if (gadget->numFiles > 1) {
		char number[LOCAL_MAX_FILEDIGITS + 2]; // +2 for '.' and '\0'
		for (int i = 0; i < gadget->numFiles; i++) {
			sprintf(number, ".%i", i);
			gadget_setFileNameOfFile(gadget, i, xstrmerge(stem, number));
		}
	} else if (gadget->numFiles == 1) {
		gadget_setFileNameOfFile(gadget, 0, xstrdup(stem));
	}
}

extern void
gadget_setFileEndianess(gadget_t gadget, endian_t endianess)
{
	assert(gadget != NULL);

	if (endianess != endian_getSystemEndianess())
		gadget->doByteSwap = true;
	else
		gadget->doByteSwap = false;
}

extern void
gadget_setHeaderOfFile(gadget_t       gadget,
                       int            fileNumber,
                       gadgetHeader_t header)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);

	if (gadget->headers[fileNumber] != NULL)
		gadgetHeader_del(&(gadget->headers[fileNumber]));
	gadget->headers[fileNumber] = header;
}

extern void
gadget_setTOCOfFile(gadget_t    gadget,
                    int         fileNumber,
                    gadgetTOC_t toc)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);

	if (gadget->tocs[fileNumber] != NULL)
		gadgetTOC_del(&(gadget->tocs[fileNumber]));
	gadget->tocs[fileNumber] = toc;
	if (toc != NULL)
		gadgetTOC_setFileVersion(toc, gadget_getFileVersion(gadget));
}

/*--- Getter ------------------------------------------------------------*/
extern int
gadget_getNumFiles(const gadget_t gadget)
{
	assert(gadget != NULL);

	return gadget->numFiles;
}

extern gadgetVersion_t
gadget_getFileVersion(const gadget_t gadget)
{
	assert(gadget != NULL);

	return gadget->fileVersion;
}

extern const char *
gadget_getFileNameOfFile(const gadget_t gadget, int fileNumber)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);

	return gadget->fileNames[fileNumber];
}

extern endian_t
gadget_getFileEndianess(const gadget_t gadget)
{
	assert(gadget != NULL);

	endian_t systemEndianess = endian_getSystemEndianess();

	if (gadget->doByteSwap) {
		if (systemEndianess == ENDIAN_LITTLE)
			return ENDIAN_BIG;

		return ENDIAN_LITTLE;
	}

	return systemEndianess;
}

extern gadgetHeader_t
gadget_getHeaderOfFile(const gadget_t gadget, int fileNumber)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);

	return gadget->headers[fileNumber];
}

extern gadgetTOC_t
gadget_getTOCOfFile(const gadget_t gadget, int fileNumber)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);

	return gadget->tocs[fileNumber];
}

/*--- IO: Basic ---------------------------------------------------------*/
extern void
gadget_open(gadget_t gadget, gadget_mode_t mode, int fileNumber)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);

	if (gadget->f != NULL) {
		if ((gadget->lastOpened != fileNumber) || (gadget->mode != mode)) {
			xfclose(&(gadget->f));
		} else if ((gadget->lastOpened == fileNumber)
		           && (gadget->mode == mode)) {
			rewind(gadget->f);
			return;
		}
	}

	if (mode == GADGET_MODE_READ) {
		gadget->f = xfopen(gadget->fileNames[fileNumber], "rb");
	} else if (mode == GADGET_MODE_WRITE_CONT) {
		gadget->f = fopen(gadget->fileNames[fileNumber], "r+b");
		if (gadget->f == NULL)
			gadget->f = xfopen(gadget->fileNames[fileNumber], "w+b");
	} else {
		gadget->f = xfopen(gadget->fileNames[fileNumber], "w+b");
	}

	gadget->mode       = mode;
	gadget->lastOpened = fileNumber;
}

extern void
gadget_close(gadget_t gadget)
{
	assert(gadget != NULL);

	if (gadget->f != NULL)
		xfclose(&(gadget->f));
}

extern void
gadget_createEmptyFile(gadget_t gadget, int fileNumber)
{
	assert(gadget != NULL);
	assert(fileNumber >= 0 && fileNumber < gadget->numFiles);
	assert(gadget->tocs[fileNumber] != NULL);
	assert(gadgetTOC_isValid(gadget->tocs[fileNumber]));

	size_t totalFileSize;
	totalFileSize = gadgetTOC_getTotalFileSize(gadget->tocs[fileNumber]);

	xfile_createFileWithSize(gadget->fileNames[fileNumber], totalFileSize);
}

/*--- IO: Writing (Fileset) ---------------------------------------------*/
extern uint64_t
gadget_writeBlock(gadget_t      gadget,
                  gadgetBlock_t block,
                  uint64_t      pSkip,
                  uint64_t      pWrite,
                  const stai_t  stai)
{
	assert(gadget != NULL);
	assert(block != GADGETBLOCK_UNKNOWN);
	assert(block != GADGETBLOCK_HEAD);
	assert(stai != NULL);

	uint64_t pWriteActual = UINT64_C(0);
	uint64_t pSkipFile, pWriteFile, pInFile, pWriteFileCheck;
	stai_t   staiClone    = stai_clone(stai);

	for (int i = 0; i < gadget->numFiles; i++) {
		pInFile    = gadgetHeader_getNumPartsInBlock(
		    gadget->headers[i], block);
		pSkipFile  = (pSkip > pInFile) ? pInFile : pSkip;
		pWriteFile = pInFile - pSkipFile;
		pWriteFile = (pWriteFile > pWrite) ? pWrite : pWriteFile;

		if (pWriteFile > 0) {
			gadget_open(gadget, GADGET_MODE_WRITE_CONT, i);
			pWriteFileCheck = gadget_writeBlockToCurrentFile(gadget, i,
			                                                 pSkipFile,
			                                                 pWriteFile,
			                                                 staiClone);
			assert(pWriteFileCheck == pWriteFile);
			gadget_close(gadget);
			pWrite       -= pWriteFileCheck;
			pWriteActual += pWriteFileCheck;
		}

		stai_rebase(staiClone, pWriteFile);
		pSkip -= pSkipFile;
	}

	stai_del(&staiClone);

	return pWriteActual;
} /* gadget_writeBlock */

/*--- IO: Writing (Current File) ----------------------------------------*/
extern void
gadget_writeHeaderToCurrentFile(gadget_t gadget)
{
	assert(gadget != NULL);
	assert(gadget->f != NULL);
	assert((gadget->mode == GADGET_MODE_WRITE_CREATE)
	       || (gadget->mode == GADGET_MODE_WRITE_CONT));
	assert(gadget->headers[gadget->lastOpened] != NULL);
	assert(gadget->tocs[gadget->lastOpened] != NULL);

	gadgetTOC_seekToDescriptor(gadget->tocs[gadget->lastOpened],
	                           GADGETBLOCK_HEAD, gadget->f);
	gadgetBlock_writeDescriptor(gadget->f,
	                            GADGETBLOCK_HEAD,
	                            GADGETHEADER_SIZE,
	                            gadget->doByteSwap,
	                            gadget->fileVersion);
	gadgetHeader_write(gadget->headers[gadget->lastOpened],
	                   gadget->f,
	                   gadget->doByteSwap);
}

extern uint64_t
gadget_writeBlockToCurrentFile(gadget_t      gadget,
                               gadgetBlock_t block,
                               uint32_t      pSkipFile,
                               uint32_t      pWriteFile,
                               const stai_t  stai)
{
	assert(gadget != NULL);
	assert(gadget->f != NULL);
	assert((gadget->mode == GADGET_MODE_WRITE_CREATE)
	       || (gadget->mode == GADGET_MODE_WRITE_CONT));
	assert(gadget->headers[gadget->lastOpened] != NULL);
	assert(gadget->tocs[gadget->lastOpened] != NULL);
	assert(gadgetTOC_isValid(gadget->tocs[gadget->lastOpened]));
	assert(gadgetTOC_blockExists(gadget->tocs[gadget->lastOpened], block));
	assert(block != GADGETBLOCK_UNKNOWN);
	assert(block != GADGETBLOCK_HEAD);
	assert(stai != NULL);

	const gadgetHeader_t head = gadget->headers[gadget->lastOpened];
	const gadgetTOC_t    toc  = gadget->tocs[gadget->lastOpened];
	uint32_t             bs   = gadgetTOC_getSizeInBytesForBlock(toc, block);
	uint32_t             nPiB = gadgetHeader_getNumPartsInBlock(head, block);
	size_t               sOE  = gadgetHeader_sizeOfElement(head, block);
	uint32_t             nC   = gadgetBlock_getNumComponents(block);

	assert(bs == nPiB * sOE);
	assert(pSkipFile + pWriteFile <= nPiB);

	gadgetTOC_seekToDescriptor(toc, block, gadget->f);
	gadgetBlock_writeDescriptor(gadget->f, block, bs, gadget->doByteSwap,
	                            gadget->fileVersion);

	gadgetBlock_writeBlockSize(gadget->f, bs, gadget->doByteSwap);
	xfseek(gadget->f, pSkipFile * sOE, SEEK_CUR);

	local_writeBlockActual(gadget->f, stai, gadget->doByteSwap, pWriteFile,
	                       sOE, nC, gadgetBlock_isInteger(block));

	xfseek(gadget->f, (nPiB - pSkipFile - pWriteFile) * sOE, SEEK_CUR);
	gadgetBlock_writeBlockSize(gadget->f, bs, gadget->doByteSwap);

	return pWriteFile;
} /* gadget_writeBlockToCurrentFile */

/*--- IO: Reading (File set) --------------------------------------------*/
extern uint64_t
gadget_readBlock(gadget_t      gadget,
                 gadgetBlock_t block,
                 uint64_t      pSkip,
                 uint64_t      pRead,
                 stai_t        stai)
{
	assert(gadget != NULL);
	assert(block != GADGETBLOCK_UNKNOWN);
	assert(block != GADGETBLOCK_HEAD);
	assert(stai != NULL);

	uint64_t pSkipFile, pReadFile, pInFile, pReadFileCheck;
	uint64_t pReadActual = 0;
	stai_t   staiClone   = stai_clone(stai);

	for (int i = 0; i < gadget->numFiles; i++) {
		pInFile   = gadgetHeader_getNumPartsInBlock(
		    gadget->headers[i], block);
		pSkipFile = (pSkip > pInFile) ? pInFile : pSkip;
		pReadFile = pInFile - pSkipFile;
		pReadFile = (pReadFile > pRead) ? pRead : pReadFile;

		if (pReadFile > 0) {
			gadget_open(gadget, GADGET_MODE_READ, i);
			pReadFileCheck = gadget_readBlockFromCurrentFile(gadget, block,
			                                                 pSkipFile,
			                                                 pReadFile,
			                                                 staiClone);
			assert(pReadFileCheck == pReadFile);
			gadget_close(gadget);
			pRead       -= pReadFileCheck;
			pReadActual += pReadFileCheck;
		}

		stai_rebase(staiClone, pReadFile);
		pSkip       -= pSkipFile;
	}

	stai_del(&staiClone);

	return pReadActual;
} /* gadget_readBlock */

/*--- IO: Reading (Current file) ----------------------------------------*/
extern void
gadget_readTOCFromCurrentFile(gadget_t gadget)
{
	assert(gadget != NULL);
	assert(gadget->f != NULL);
	assert(gadget->mode == GADGET_MODE_READ);

	if (gadget->tocs[gadget->lastOpened] != NULL)
		gadgetTOC_del(&(gadget->tocs[gadget->lastOpened]));
	gadget->tocs[gadget->lastOpened] = gadgetTOC_newFromFile(gadget->f);
}

extern void
gadget_readHeaderFromCurrentFile(gadget_t gadget)
{
	assert(gadget != NULL);
	assert(gadget->f != NULL);
	assert(gadget->mode == GADGET_MODE_READ);
	assert(gadgetTOC_isValid(gadget->tocs[gadget->lastOpened]));

	gadgetTOC_seekToData(gadget->tocs[gadget->lastOpened],
	                     GADGETBLOCK_HEAD, gadget->f);

	if (gadget->headers[gadget->lastOpened] == NULL)
		gadget->headers[gadget->lastOpened] = gadgetHeader_new();
	gadgetHeader_read(gadget->headers[gadget->lastOpened], gadget->f,
	                  gadget->doByteSwap);
}

extern void
gadget_detectByteswappingOfCurrentFile(gadget_t gadget)
{
	assert(gadget != NULL);
	assert(gadget->f != NULL);
	assert(gadget->mode == GADGET_MODE_READ);

	endian_t endianessFile;

	endianessFile = endian_getFileEndianessByBlockF(gadget->f);

	if (endianessFile == endian_getSystemEndianess())
		gadget->doByteSwap = false;
	else
		gadget->doByteSwap = true;
}

extern void
gadget_detectFileVersionOfCurrentFile(gadget_t gadget)
{
	assert(gadget != NULL);
	assert(gadget->f != NULL);
	assert(gadget->mode == GADGET_MODE_READ);

	long     oldPos = xftell(gadget->f);
	uint32_t blockSize;

	rewind(gadget->f);

	gadgetBlock_readBlockSize(gadget->f, &blockSize, gadget->doByteSwap);

	if (blockSize == GADGETBLOCK_DESCRIPTOR_BLOCK_LENGTH)
		gadget_setFileVersion(gadget, GADGETVERSION_TWO);
	else if (blockSize == GADGETHEADER_SIZE)
		gadget_setFileVersion(gadget, GADGETVERSION_ONE);
	else
		diediedie(EXIT_FAILURE);

	xfseek(gadget->f, oldPos, SEEK_SET);
}

extern uint64_t
gadget_readBlockFromCurrentFile(gadget_t      gadget,
                                gadgetBlock_t block,
                                uint64_t      pSkipFile,
                                uint64_t      pReadFile,
                                stai_t        stai)
{
	assert(gadget != NULL);
	assert(gadget->headers[gadget->lastOpened] != NULL);
	assert(gadget->tocs[gadget->lastOpened] != NULL);
	assert(gadgetTOC_isValid(gadget->tocs[gadget->lastOpened]));
	assert(gadgetTOC_blockExists(gadget->tocs[gadget->lastOpened], block));
	assert(block != GADGETBLOCK_UNKNOWN);
	assert(block != GADGETBLOCK_HEAD);
	assert(stai != NULL);

	uint32_t             bsF;
	const gadgetHeader_t head = gadget->headers[gadget->lastOpened];
	const gadgetTOC_t    toc  = gadget->tocs[gadget->lastOpened];
	uint32_t             bs   = gadgetTOC_getSizeInBytesForBlock(toc, block);
	uint32_t             nPiB = gadgetHeader_getNumPartsInBlock(head, block);
	size_t               sOE  = gadgetHeader_sizeOfElement(head, block);
	uint32_t             nC   = gadgetBlock_getNumComponents(block);

	if (bs != nPiB * sOE) {
		if (bs == nPiB * sOE * 2) {
			sOE *= 2;
		} else {
			diediedie(EXIT_FAILURE);
		}
	}
	assert(pSkipFile + pReadFile <= nPiB);

	gadgetTOC_seekToData(toc, block, gadget->f);

	gadgetBlock_readBlockSize(gadget->f, &bsF, gadget->doByteSwap);
	assert(bsF == bs);
	xfseek(gadget->f, pSkipFile * sOE, SEEK_CUR);

	local_readBlockActual(gadget->f, stai, gadget->doByteSwap, pReadFile,
	                      sOE, nC, gadgetBlock_isInteger(block));

	xfseek(gadget->f, (nPiB - pSkipFile - pReadFile) * sOE, SEEK_CUR);
	gadgetBlock_readBlockSize(gadget->f, &bsF, gadget->doByteSwap);
	assert(bsF == bs);

	return pReadFile;
} /* gadget_readBlockFromCurrentFile */

/*--- Implementations of local functions --------------------------------*/
static void
local_shrinkADTArrays(gadget_t gadget, int numFiles)
{
	assert(gadget->numFiles > numFiles);

	for (int i = numFiles; i < gadget->numFiles; i++) {
		if (gadget->fileNames[i] != NULL)
			xfree(gadget->fileNames[i]);
		if (gadget->headers[i] != NULL)
			gadgetHeader_del(&(gadget->headers[i]));
		if (gadget->tocs[i] != NULL)
			gadgetTOC_del(&(gadget->tocs[i]));
	}
	if (numFiles > 0) {
		gadget->fileNames = xrealloc(gadget->fileNames,
		                             sizeof(char *) * numFiles);
		gadget->headers   = xrealloc(gadget->headers,
		                             sizeof(gadgetHeader_t *) * numFiles);
		gadget->tocs      = xrealloc(gadget->tocs,
		                             sizeof(gadgetTOC_t *) * numFiles);
	} else {
		xfree(gadget->fileNames);
		xfree(gadget->headers);
		xfree(gadget->tocs);
	}
	gadget->numFiles = numFiles;
}

static void
local_enlargeADTArrays(gadget_t gadget, int numFiles)
{
	assert(gadget->numFiles >= 0);
	assert(gadget->numFiles < numFiles);

	if (gadget->numFiles <= 0) {
		gadget->fileNames = xmalloc(sizeof(char *) * numFiles);
		gadget->headers   = xmalloc(sizeof(gadgetHeader_t *) * numFiles);
		gadget->tocs      = xmalloc(sizeof(gadgetTOC_t *) * numFiles);
	} else {
		gadget->fileNames = xrealloc(gadget->fileNames,
		                             sizeof(char *) * numFiles);
		gadget->headers   = xrealloc(gadget->headers,
		                             sizeof(gadgetHeader_t *) * numFiles);
		gadget->tocs      = xrealloc(gadget->tocs,
		                             sizeof(gadgetTOC_t *) * numFiles);
	}
	for (int i = gadget->numFiles; i < numFiles; i++) {
		gadget->fileNames[i] = NULL;
		gadget->headers[i]   = NULL;
		gadget->tocs[i]      = NULL;
	}
	gadget->numFiles = numFiles;
}

inline static void
local_skipToReqPosInBlock(FILE *f, uint64_t pSkip, size_t sizeOfElement)
{
	xfseek(f, (long)(pSkip * sizeOfElement), SEEK_CUR);
}

inline static void
local_writeBlockActual(FILE         *f,
                       const stai_t stai,
                       bool         doByteSwap,
                       uint32_t     pWrite,
                       size_t       sizeOfElementFile,
                       int          numComponents,
                       bool         isInteger)
{
	size_t sizeOfElementStai = (size_t)stai_getSizeOfElementInBytes(stai);

	if ((sizeOfElementFile == sizeOfElementStai) && !doByteSwap
	    && stai_isLinear(stai)) {
		xfwrite(stai_getBase(stai), sizeOfElementStai, pWrite, f);
	} else {
		local_writeBlockActualGeneral(f, pWrite, doByteSwap, stai,
		                              sizeOfElementFile, sizeOfElementStai,
		                              numComponents, isInteger);
	}
}

static void
local_writeBlockActualGeneral(FILE         *f,
                              uint64_t     pWrite,
                              bool         doByteSwap,
                              const stai_t stai,
                              size_t       sizeOfElement,
                              size_t       sizeOfElementStai,
                              int          numComponents,
                              bool         isInteger)
{
	char ele[sizeOfElement];

	if (sizeOfElement > sizeOfElementStai) {
		assert(sizeOfElement == 2 * sizeOfElementStai);
		char eleStai[sizeOfElementStai];
		for (uint64_t i = 0; i < pWrite; i++) {
			stai_getElement(stai, i, eleStai);
			if (doByteSwap)
				byteswapVec(eleStai, sizeOfElementStai, numComponents);
			local_upcastElement(eleStai, ele, numComponents, isInteger);
			xfwrite(ele, sizeOfElement, 1, f);
		}
	} else if (sizeOfElement < sizeOfElementStai) {
		assert(sizeOfElement * 2 == sizeOfElementStai);
		char eleStai[sizeOfElementStai];
		for (uint64_t i = 0; i < pWrite; i++) {
			stai_getElement(stai, i, eleStai);
			if (doByteSwap)
				byteswapVec(eleStai, sizeOfElementStai, numComponents);
			local_downcastElement(eleStai, ele, numComponents, isInteger);
			xfwrite(ele, sizeOfElement, 1, f);
		}
	} else {
		for (uint64_t i = 0; i < pWrite; i++) {
			stai_getElement(stai, i, ele);
			if (doByteSwap)
				byteswapVec(ele, sizeOfElement, numComponents);
			xfwrite(ele, sizeOfElement, 1, f);
		}
	}
}

inline static void
local_readBlockActual(FILE     *f,
                      stai_t   stai,
                      bool     doByteSwap,
                      uint64_t pRead,
                      size_t   sizeOfElementFile,
                      int      numComponents,
                      bool     isInteger)
{
	size_t sizeOfElementStai = stai_getSizeOfElementInBytes(stai);

	assert(sizeOfElementStai % numComponents == 0);

	if ((!doByteSwap) && stai_isLinear(stai)
	    && (sizeOfElementFile == sizeOfElementStai)) {
		xfread(stai_getBase(stai), sizeOfElementFile, pRead, f);
	} else {
		local_readBlockActualGeneral(f, pRead, doByteSwap, stai,
		                             sizeOfElementFile, sizeOfElementStai,
		                             numComponents, isInteger);
	}
}

static void
local_readBlockActualGeneral(FILE     *f,
                             uint64_t pRead,
                             bool     doByteSwap,
                             stai_t   stai,
                             size_t   sizeOfElement,
                             size_t   sizeOfElementStai,
                             int      numComponents,
                             bool     isInteger)
{
	char ele[sizeOfElement];

	if (sizeOfElement > sizeOfElementStai) {
		assert(sizeOfElement == 2 * sizeOfElementStai);
		char eleStai[sizeOfElementStai];
		for (uint64_t i = 0; i < pRead; i++) {
			xfread(ele, sizeOfElement, 1, f);
			if (doByteSwap)
				byteswapVec(ele, sizeOfElement, numComponents);
			local_downcastElement(ele, eleStai, numComponents, isInteger);
			stai_setElement(stai, i, eleStai);
		}
	} else if (sizeOfElement < sizeOfElementStai) {
		assert(sizeOfElement * 2 == sizeOfElementStai);
		char eleStai[sizeOfElementStai];
		for (uint64_t i = 0; i < pRead; i++) {
			xfread(ele, sizeOfElement, 1, f);
			if (doByteSwap)
				byteswapVec(ele, sizeOfElement, numComponents);
			local_upcastElement(ele, eleStai, numComponents, isInteger);
			stai_setElement(stai, i, eleStai);
		}
	} else {
		for (uint64_t i = 0; i < pRead; i++) {
			xfread(ele, sizeOfElement, 1, f);
			if (doByteSwap)
				byteswapVec(ele, sizeOfElement, numComponents);
			stai_setElement(stai, i, ele);
		}
	}
}

inline static void
local_downcastElement(const void *restrict eleHi,
                      void *restrict       eleLo,
                      int                  numComponents,
                      bool                 isInteger)
{
	if (isInteger) {
		for (int i = 0; i < numComponents; i++)
			((uint32_t *)eleLo)[i] = (uint32_t)(((uint64_t *)eleHi)[i]);
	} else {
		for (int i = 0; i < numComponents; i++)
			((float *)eleLo)[i] = (float)(((double *)eleHi)[i]);
	}
}

inline static void
local_upcastElement(const void *restrict eleLo,
                    void *restrict       eleHi,
                    int                  numComponents,
                    bool                 isInteger)
{
	if (isInteger) {
		for (int i = 0; i < numComponents; i++)
			((uint64_t *)eleHi)[i] = (uint64_t)(((uint32_t *)eleLo)[i]);
	} else {
		for (int i = 0; i < numComponents; i++)
			((double *)eleHi)[i] = (double)(((float *)eleLo)[i]);
	}
}
