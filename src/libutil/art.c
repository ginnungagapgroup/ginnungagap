// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/art.c
 * @ingroup libutilFilesART
 * @brief  This file provides the implementation of the ART file object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "art.h"
#include "artHeader.h"
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include "xmem.h"
#include "xfile.h"
#include "xstring.h"
#include "stai.h"
#include "byteswap.h"
#include "diediedie.h"


/*--- Implementation of main structure ----------------------------------*/
#include "art_adt.h"


/*--- Local defines -----------------------------------------------------*/

/** @brief  This is the default path for the files. */
#define ART_DEFAULT_PATH_STRING "./"

/** @brief  This is the default suffix for the files. */
#define ART_DEFAULT_SUFFIX_STRING ".DAT"

/**
 * @brief  Gives the number of digits for the file numbering.
 */
#define ART_MAX_FILEDIGITS 7

/**
 * @brief  This gives the largest number of files that should be in one
 *         set.
 *
 * The largest allowed file number (#ART_MAX_NUMFILES - 1) must not
 * have more than #ART_MAX_FILEDIGITS digits.
 */
#define ART_MAX_NUMFILES 10000000

/** @brief  Gives the largest value allowed for nrowc. */
#define ART_MAX_NROWC 1024

/**
 * @brief  Gives the number of bytes used for one particle in the file.
 *
 * This is given in bytes and simply number of componentes (6: @a x,
 * @a y, @a z, @a vx, @a vy, @a vz) times the storage size (4: all
 * values are stored as @c float).
 */
#define ART_SIZEOF_PARTICLE 24


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Constructs the file name of the PMcrd.DAT file.
 *
 * @param[in]  *pathToFiles
 *                The path to the file.
 * @param[in]  *fileNameSuffix
 *                The suffix of the file.
 *
 * @return  Returns pathToFile + PMcrd + fileNameSuffix.
 */
static char *
local_createFileNameHeader(const char *pathToFiles,
                           const char *fileNameSuffix);


/**
 * @brief  Creates an array holding all of the PMcrsX.DAT file names.
 *
 * @param[in]  *pathToFiles
 *                Gives the path to the files.
 * @param[in]  *fileNameSuffix
 *                Gives the suffix of the files.
 * @param[in]  numFiles
 *                Gives the number of files.
 *
 * @return  Returns an array of length numFiles containing the file
 *          names of the PMcrsX.DAT files.
 */
static char **
local_createFileNamesData(const char *pathToFiles,
                          const char *fileNameSuffix,
                          int        numFiles);


/**
 * @brief  Creates the file stem from a path and the first part of the
 *         file name.
 *
 * @param[in]  *path
 *                The path.  This is allowed to be
 *                #ART_USE_DEFAULT_PATH, then #ART_DEFAULT_PATH_STRING
 *                is used at the path.
 * @param[in]  *fileNameStart
 *                The first part of the filename.
 *
 * @return  Returns path + fileNameStart or path + '/' + fileNameStart
 *          depending on whether path is a proper path or is missing the
 *          trailing '/'.
 */
static char *
local_getFileStem(const char *path, const char *fileNameStart);


/**
 * @brief  Makes sure that a string ends with '/'.
 *
 * @param[in]  *path
 *                The string to use a path.
 *
 * @return  Returns either path or path + '/'.
 */
static char *
local_getProperPath(const char *path);


/**
 * @brief  Updates the numbers for the structure of the file(s).
 *
 * This should be called everytime the header is changed (actually, if
 * the number of particles or @c ngridc is changed).
 *
 * @param[in,out]  art
 *                    The ART object with a header attached from which
 *                    the numbers are calculated.
 *
 * @return  Returns nothing.
 */
static void
local_updateNumbers(art_t art);


/**
 * @brief  Writes one component to a page.
 *
 * This requires the file to be positioned at the beginning of the
 * component block that should be written.  For the skip functionality
 * to work, the whole component block must already exist in the file.
 * When writing a new block, @c pSkip @b must be 0, @c pWrite @b must
 * be equal to the number of particles in the block and @c component
 * <b>must not</b> be @c NULL.  If these restrictions are not met, the
 * behaviour is undefined, but most likely the code will terminate.
 *
 * @param[in,out]  art
 *                    The ART file object.  The correct data file must
 *                    be opened in write mode and the file pointer must
 *                    be positioned at the correct position at which to
 *                    start writing.  Passing @c NULL is undefined.
 * @param[in]      pSkip
 *                    The number of particles to skip over.  This must
 *                    be less or equal to the total number of particles
 *                    in the page.
 * @param[in]      pWrite
 *                    The number of particles to write to the file.  The
 *                    sum of @c pWrite and @c pSkip must be less or
 *                    equal to the number of particles in the page.
 * @param[in]      component
 *                    The component to write.  This must contain
 *                    sufficient values to completely fill the
 *                    components share of the page.  Passing @c NULL
 *                    allowed, then meaning that the corresponding
 *                    component block should be skipped over.
 * @param[in]      doByteswap
 *                    This toggles between writing the file in system
 *                    endianess (false) or in the opposite endianess
 *                    (true).
 *
 * @return  Returns nothing.
 */
static void
local_writeComponent(art_t    art,
                     uint64_t pSkip,
                     uint64_t pWrite,
                     stai_t   component,
                     bool     doByteswap);


/**
 * @brief  This reads a component from a page.
 *
 * This will read a consecutive subset of the particles in the page.  It
 * requires the file to be positioned at the beginning of the component
 * of interest in the page and after completion the file will always be
 * positioned at the end of the component, irregardless of how many
 * particles were actually read.  I.e. if the page contains 16 particles
 * and 3 particles should be skipped and 4 read, than the function
 * assumes that the file is positioned at the beginning of the component
 * in the page, will skip over 3 values, then read 4, and again skip
 * over 9 values.
 *
 * If the choices of parameters are such, that nothing should be read
 * (either because the data array to read into is @c NULL, or the skip
 * value is equal to the number of particles in the page), then the
 * function will simply seek to the end of the component.
 *
 * Only in the case that the data array of @c component is linear and of
 * type @c float (size of the elements 4 bytes, and the stride also 4
 * byes) no buffer needs to be allocated.  In general, a buffer large
 * enough to hold @c pRead @c float values is allocated as temporary
 * work space.  This should in most cases be negligible as the largest
 * page size of ART files is 1Mi elements, corresponding to a buffer
 * space of 4 MB.
 *
 * @param[in,out]  art
 *                    The ART file object.  The correct data file must
 *                    be opened in read mode and the file pointer must
 *                    be positioned at the correct position from which
 *                    to start reading.  Passing @c NULL is undefined.
 * @param[in]      pSkip
 *                    The particles to skip in the file.  This must be
 *                    less or equal to the number of particles in the
 *                    page.
 * @param[in]      pRead
 *                    The number of particles to actually read from the
 *                    page.  The sum of @c pSkip and @c pRead must be
 *                    less or equal to the number of particles in the
 *                    page.
 * @param[in,out]  component
 *                    The component to read into.  This must be large
 *                    enough.  Passing @c NULL is valid and then the
 *                    file will simply seek to a position as if the
 *                    component would have been read.
 * @param[in]      doByteswap
 *                    Decides whether the values that have been read
 *                    should be byteswapped (true) or not (false).
 *
 * @return Returns nothing.
 */
static void
local_readComponent(art_t    art,
                    uint64_t pSkip,
                    uint64_t pRead,
                    stai_t   component,
                    bool     doByteswap);


/**
 * @brief  Fills a buffer from a component array.
 *
 * This will copy from a stai to a linear array, taking into account
 * possible casts to float precision (the stai may contain the values in
 * double precision).
 *
 * @param[out]  *buffer
 *                 The buffer to fill.  This must be large enough to
 *                 hold @c numValues elements.  Passing @c NULL is
 *                 undefined.
 * @param[in]   stai
 *                 The stai from which to fill the buffer.
 * @param[in]   numValues
 *                 The number of elements to copy from the stai to the
 *                 buffer.
 *
 * @return  Returns nothing.
 */
static void
local_fillBufferFromStai(float *buffer, stai_t stai, int numValues);


/**
 * @brief  Copies the values in a buffer array into a stai.
 *
 * This deals with precision casting.
 *
 * @param[in]      *buffer
 *                    The buffer array which should be copied to the
 *                    stai.  The must contain at least @c numValues
 *                    elements and must not be @c NULL.
 * @param[in,out]  stai
 *                    The stai to copy the values to.  The underlying
 *                    array must be large enough to receive the values.
 *                    Passing @c NULL is undefined.
 * @param[in]      numValues
 *                    The number of values to copy.
 */
static void
local_copyBufferToStai(const float *buffer, stai_t stai, int numValues);


/**
 * @brief  Seeks to a given page in the file.
 *
 * @param[in,out]  art
 *                    The ART file object in which to seek.  It requires
 *                    a file to be opened.  Passing @c NULL is
 *                    undefined.
 * @param[in]      pageNumber
 *                    The page to seek to.  The page itself does not
 *                    need to exist, but all pages before that are
 *                    required to exist.
 */
static void
local_seekToPage(art_t art, int pageNumber);

/**
 * @brief  Resets the base of the data stais.
 *
 * This apply a correction to the base of the data stai to take into
 * account the number of particles read/written.
 *
 * @param[in,out]  *data
 *                    The stais to offset.  This array must hold at
 *                    least 6 stais and only the first 6 are used.  Any
 *                    of those may be @c NULL.
 * @param[in]      offsetElements
 *                    The number of elements to offset the base element.
 *                    This offset may be negative, according with
 *                    whether a later element (positive) or an earlier
 *                    element (negative) should be the new base element.
 *
 * @return  Returns nothing.
 */
inline static void
local_rebaseStaisData(stai_t *data, int64_t offsetElements);

/**
 * @brief  Helper function to set the first/last file/page depending on
 *         the number of particles per file/page.
 *
 * @param[in]   normalizer
 *                 The number of elements in the file/page.
 * @param[in]   largest
 *                 The largest file/page number, neither <tt>*first</tt>
 *                 nor <tt>*last</tt> will exceed this value.
 * @param[in]   pSkip
 *                 The number of particles to skip.
 * @param[in]   pAct
 *                 The number of particles to act upon.
 * @param[out]  *first
 *                 Will receive the first page/file number that has
 *                 particles on which to act.
 * @param[out]  *last
 *                 Will receive the last page/file number that has
 *                 particles on which to act.
 *
 * @return  Returns nothing.
 */
static void
local_calcFirstLast(int      normalizer,
                    int      largest,
                    uint64_t pSkip,
                    uint64_t pAct,
                    int      *first,
                    int      *last);

/**
 * @brief  Adjust the skip and act value to conform with a limited
 *         amount of particles.
 *
 * In total we need to skip @c pSkip particles and act on @c pAct
 * particles, however the file/page only contains @c normalizer
 * particles.  This function will return new skip and act values that
 * are valid for this limited amount of particles.
 *
 * This assumes that @c pSkip and @c pAct are adjusted accordingly once
 * the file/page has been processed to account for the particles that
 * have just been skipped/acted upon.
 *
 * @param[in]   normalizer
 *                 The number of particles available in the file/page.
 * @param[in]   pSkip
 *                 The number of particles that should be skipped in
 *                 total.
 * @param[in]   pAct
 *                 The number of particles that should be acted on in
 *                 total.
 * @param[out]  *pSkipCalc
 *                 Receives the number of particles to skip in the
 *                 file/page.
 * @param[out]  *pActCalc
 *                 Receives the number of particles to act on in the
 *                 file/page.
 *
 * @return  Return nothing.
 */
static void
local_calcSkipAct(int      normalizer,
                  uint64_t pSkip,
                  uint64_t pAct,
                  uint64_t *pSkipCalc,
                  uint64_t *pActCalc);

/**
 * @brief  Calculates how many digits an integer value has.
 *
 * @param[in]  numFiles
 *                The number of files, this must be a positive value.
 *
 * @return  Returns the number of digits, but not more than
 *          #ART_MAX_FILEDIGITS.
 */
static int
local_getRequiredDigits(int numFiles);


/*--- Implementations of exported functions -----------------------------*/
extern art_t
art_new(const char *pathToFiles, const char *fileNameSuffix, int numFiles)
{
	art_t art;

	assert(numFiles > 0 && numFiles < ART_MAX_NUMFILES);

	art                 = xmalloc(sizeof(struct art_struct));
	art->fileNameHeader = local_createFileNameHeader(pathToFiles,
	                                                 fileNameSuffix);
	art->fileNamesData  = local_createFileNamesData(pathToFiles,
	                                                fileNameSuffix,
	                                                numFiles);
	art->numFiles               = numFiles;
	art->truncateNrowc          = true;
	art->f                      = NULL;
	art->mode                   = ART_MODE_READ;
	art->lastOpened             = -1;
	art->header                 = NULL;
	art->numTotalPages          = -1;
	art->numPagesInFile         = -1;
	art->numPagesInLastFile     = -1;
	art->numParticlesInPage     = -1;
	art->numParticlesInLastPage = -1;
	art->numParticlesInFile     = -1;
	art->numParticlesInLastFile = -1;
	art->numPagesInThisFile     = -1;
	art->numParticlesInThisFile = -1;

	return art;
}

extern void
art_del(art_t *art)
{
	assert(art != NULL && *art != NULL);

	if ((*art)->header != NULL)
		artHeader_del(&((*art)->header));
	art_close(*art);
	if ((*art)->fileNamesData != NULL) {
		for (int i = 0; i < (*art)->numFiles; i++) {
			if ((*art)->fileNamesData[i] != NULL)
				xfree((*art)->fileNamesData[i]);
		}
		xfree((*art)->fileNamesData);
	}
	if ((*art)->fileNameHeader != NULL)
		xfree((*art)->fileNameHeader);

	xfree(*art);
	art = NULL;
}

extern void
art_setTruncateNrowc(art_t art, bool value)
{
	assert(art != NULL);

	art->truncateNrowc = value;
	if (art->header != NULL)
		local_updateNumbers(art);
}

extern int
art_getNumFiles(const art_t art)
{
	assert(art != NULL);

	return art->numFiles;
}

extern bool
art_getTruncateNrowc(const art_t art)
{
	assert(art != NULL);

	return art->truncateNrowc;
}

extern const char *
art_getHeaderFileName(const art_t art)
{
	assert(art != NULL);

	return art->fileNameHeader;
}

extern const char *
art_getDataFileName(const art_t art, int numFile)
{
	assert(art != NULL);
	assert(numFile >= 0 && numFile < art->numFiles);

	return art->fileNamesData[numFile];
}

extern artHeader_t
art_getHeaderHandle(const art_t art)
{
	assert(art != NULL);

	return art->header;
}

extern void
art_attachHeader(art_t art, artHeader_t header)
{
	assert(art != NULL);
	assert(header != NULL);

	if (art->header != NULL)
		artHeader_del(&(art->header));
	art->header = header;

	local_updateNumbers(art);
}

extern void
art_attachHeaderFromFile(art_t art)
{
	artHeader_t header;

	assert(art != NULL);
	assert(art->fileNameHeader != NULL);

	header = artHeader_newFromFile(art->fileNameHeader);

	art_attachHeader(art, header);
}

extern void
art_open(art_t art, artMode_t mode, int numFile)
{
	assert(art != NULL);
	assert(numFile >= 0 && numFile < art->numFiles);

	if (art->f != NULL) {
		if ((art->lastOpened != numFile) || (mode != art->mode))
			xfclose(&(art->f));
		else if ((art->lastOpened == numFile) && (mode == art->mode)) {
			rewind(art->f);
			return;
		}
	}

	art->mode = mode;
	if (art->mode == ART_MODE_READ) {
		art->f = xfopen(art->fileNamesData[numFile], "rb");
	} else if (art->mode == ART_MODE_WRITE) {
		art->f = xfopen(art->fileNamesData[numFile], "r+b");
	} else {
		diediedie(EXIT_FAILURE);
	}

	art->lastOpened         = numFile;
	art->numPagesInThisFile = (numFile == art->numFiles - 1)
	                          ? art->numParticlesInLastPage
							  : art->numParticlesInPage;
	art->numParticlesInThisFile = (numFile == art->numFiles - 1)
	                              ? art->numParticlesInLastFile
								  : art->numParticlesInFile;
}

extern void
art_createEmptyFile(const art_t art, int numFile)
{
	size_t numPagesTotal;

	assert(art != NULL);
	assert(numFile < art->numFiles);

	if (numFile == art->numFiles - 1)
		numPagesTotal = art->numPagesInLastFile;
	else
		numPagesTotal = art->numPagesInFile;

	xfile_createFileWithSize(art->fileNamesData[numFile],
	                         (size_t)art->numParticlesInPage
	                         * (size_t)ART_SIZEOF_PARTICLE
	                         * numPagesTotal);
}

extern void
art_close(art_t art)
{
	assert(art != NULL);

	if (art->f != NULL)
		xfclose(&(art->f));
	art->numPagesInThisFile     = -1;
	art->numParticlesInThisFile = -1;
}

extern uint64_t
art_writeToPage(art_t    art,
                int      pageNumber,
                uint64_t pSkip,
                uint64_t pWrite,
                stai_t   *data)
{
	bool     doByteswap      = false;
	endian_t systemEndianess = endian_getSystemEndianess();

	assert(art != NULL);
	assert(art->f != NULL);
	assert(art->mode == ART_MODE_WRITE);
	assert(pageNumber < art->numPagesInThisFile);

	local_seekToPage(art, pageNumber);

	if (systemEndianess != artHeader_getFileEndianess(art->header))
		doByteswap = true;

	local_writeComponent(art, pSkip, pWrite, data[0], doByteswap);
	local_writeComponent(art, pSkip, pWrite, data[1], doByteswap);
	local_writeComponent(art, pSkip, pWrite, data[2], doByteswap);
	local_writeComponent(art, pSkip, pWrite, data[3], doByteswap);
	local_writeComponent(art, pSkip, pWrite, data[4], doByteswap);
	local_writeComponent(art, pSkip, pWrite, data[5], doByteswap);

	return pWrite;
}

extern uint64_t
art_writeToFile(art_t    art,
                int      fileNumber,
                uint64_t pSkip,
                uint64_t pWrite,
                stai_t   *data)
{
	bool     wasOpened;
	uint64_t numPartsWriteTotal = UINT64_C(0);
	int      firstPage, lastPage;

	wasOpened = (art->f == NULL) ? false : true;
	art_open(art, ART_MODE_WRITE, fileNumber);

	local_calcFirstLast(art->numParticlesInPage, art->numPagesInThisFile,
	                    pSkip, pWrite, &firstPage, &lastPage);
	pSkip -= ((uint64_t)firstPage) * art->numParticlesInPage;

	for (int i = firstPage; i <= lastPage; i++) {
		uint64_t pSkipPage, pWritePage, actualWrite;

		local_calcSkipAct(art->numParticlesInPage, pSkip, pWrite,
		                  &pSkipPage, &pWritePage);
		actualWrite = art_writeToPage(art, i, pSkipPage, pWritePage, data);
		assert(actualWrite == pWritePage);

		local_rebaseStaisData(data, actualWrite);
		numPartsWriteTotal += actualWrite;
		pSkip              -= pSkipPage;
		pWrite             -= pWritePage;
	}
	local_rebaseStaisData(data, -((int64_t)numPartsWriteTotal));

	if (!wasOpened)
		art_close(art);

	return numPartsWriteTotal;
}

extern uint64_t
art_write(art_t    art,
          uint64_t pSkip,
          uint64_t pWrite,
          stai_t   *data)
{
	uint64_t numPartsWriteTotal = UINT64_C(0);
	int      firstFile, lastFile;

	assert(art != NULL);
	assert(pSkip + pWrite <= artHeader_getNumParticlesTotal(art->header));

	local_calcFirstLast(art->numParticlesInFile, art->numFiles - 1,
	                    pSkip, pWrite, &firstFile, &lastFile);
	pSkip -= ((uint64_t)firstFile) * art->numParticlesInFile;

	for (int i = firstFile; i <= lastFile; i++) {
		uint64_t pSkipFile, pWriteFile, actualWrite;

		art_open(art, ART_MODE_WRITE, i);
		local_calcSkipAct(art->numParticlesInThisFile,
		                  pSkip, pWrite, &pSkipFile, &pWriteFile);
		actualWrite = art_writeToFile(art, i, pSkipFile, pWriteFile, data);
		assert(actualWrite == pWriteFile);
		art_close(art);

		local_rebaseStaisData(data, actualWrite);
		numPartsWriteTotal += actualWrite;
		pSkip              -= pSkipFile;
		pWrite             -= pWriteFile;
	}
	local_rebaseStaisData(data, -((int64_t)numPartsWriteTotal));

	return numPartsWriteTotal;
}

extern uint64_t
art_readFromPage(art_t    art,
                 int      pageNumber,
                 uint64_t pSkip,
                 uint64_t pRead,
                 stai_t   *data)
{
	bool     doByteswap      = false;
	endian_t systemEndianess = endian_getSystemEndianess();

	assert(art != NULL);
	assert(art->f != NULL && art->mode == ART_MODE_READ);
	assert(pageNumber < art->numPagesInThisFile);

	local_seekToPage(art, pageNumber);

	if (systemEndianess != artHeader_getFileEndianess(art->header))
		doByteswap = true;

	local_readComponent(art, pSkip, pRead, data[0], doByteswap);
	local_readComponent(art, pSkip, pRead, data[1], doByteswap);
	local_readComponent(art, pSkip, pRead, data[2], doByteswap);
	local_readComponent(art, pSkip, pRead, data[3], doByteswap);
	local_readComponent(art, pSkip, pRead, data[4], doByteswap);
	local_readComponent(art, pSkip, pRead, data[5], doByteswap);

	return pRead;
}

extern uint64_t
art_readFromFile(art_t    art,
                 int      fileNumber,
                 uint64_t pSkip,
                 uint64_t pRead,
                 stai_t   *data)
{
	bool     wasOpened;
	uint64_t numPartsReadTotal = UINT64_C(0);
	int      firstPage, lastPage;

	wasOpened = (art->f == NULL) ? false : true;
	art_open(art, ART_MODE_READ, fileNumber);

	local_calcFirstLast(art->numParticlesInPage, art->numPagesInThisFile,
	                    pSkip, pRead, &firstPage, &lastPage);
	pSkip -= ((uint64_t)firstPage) * art->numParticlesInPage;

	for (int i = firstPage; i <= lastPage; i++) {
		uint64_t pSkipPage, pReadPage, actualRead;

		local_calcSkipAct(art->numParticlesInPage, pSkip, pRead,
		                  &pSkipPage, &pReadPage);
		actualRead = art_readFromPage(art, i, pSkipPage, pReadPage, data);
		assert(actualRead == pReadPage);

		local_rebaseStaisData(data, actualRead);
		numPartsReadTotal += actualRead;
		pSkip             -= pSkipPage;
		pRead             -= pReadPage;
	}
	local_rebaseStaisData(data, -((int64_t)numPartsReadTotal));

	if (!wasOpened)
		art_close(art);

	return numPartsReadTotal;
}

extern uint64_t
art_read(art_t art, uint64_t pSkip, uint64_t pRead, stai_t *data)
{
	uint64_t numPartsReadTotal = UINT64_C(0);
	int      firstFile, lastFile;

	assert(art != NULL);
	assert(pSkip + pRead <= artHeader_getNumParticlesTotal(art->header));

	local_calcFirstLast(art->numParticlesInFile, art->numFiles - 1,
	                    pSkip, pRead, &firstFile, &lastFile);
	pSkip -= ((uint64_t)firstFile) * art->numParticlesInFile;

	for (int i = firstFile; i <= lastFile; i++) {
		uint64_t pSkipFile, pReadFile, actualRead;

		art_open(art, ART_MODE_READ, i);
		local_calcSkipAct(art->numParticlesInThisFile,
		                  pSkip, pRead, &pSkipFile, &pReadFile);
		actualRead = art_readFromFile(art, i, pSkipFile, pReadFile, data);
		assert(actualRead == pReadFile);
		art_close(art);

		local_rebaseStaisData(data, actualRead);
		numPartsReadTotal += actualRead;
		pSkip             -= pSkipFile;
		pRead             -= pReadFile;
	}
	local_rebaseStaisData(data, -((int64_t)numPartsReadTotal));

	return numPartsReadTotal;
}

extern void
art_prettyPrint(const art_t art, const char *prefix, FILE *f)
{
	int        i;
	const char *actualPrefix = (prefix == NULL) ? "" : prefix;


	fprintf(f, "%s\n%sGeneral information:\n", actualPrefix, actualPrefix);
	fprintf(f, "%s  Header file name                :  %s\n",
	        actualPrefix, art->fileNameHeader);
	fprintf(f, "%s  Number of files                 :  %i\n",
	        actualPrefix, art->numFiles);
	fprintf(f, "%s  Total number of pages           :  %i\n",
	        actualPrefix, art->numTotalPages);
	fprintf(f, "%s  Total number of particles       :  %" PRIu64 "\n",
	        actualPrefix, artHeader_getNumParticlesTotal(art->header));
	fprintf(f, "%s  Number of particles in page     :  %i\n",
	        actualPrefix, art->numParticlesInPage);
	fprintf(f, "%s  Number of particles in last page:  %i\n",
	        actualPrefix, art->numParticlesInLastPage);

	fprintf(f, "%s\n%sFile specific information:\n",
	        actualPrefix, actualPrefix);
	for (i = 0; i < art->numFiles - 1; i++) {
		fprintf(f, "%s  Data file name:  %s\n",
		        actualPrefix, art->fileNamesData[i]);
		fprintf(f, "%s    pages in file    :  %i\n",
		        actualPrefix, art->numPagesInFile);
		fprintf(f, "%s    particles in file:  %i\n",
		        actualPrefix, art->numParticlesInFile);
	}

	fprintf(f, "%s  Data file name:  %s\n",
	        actualPrefix, art->fileNamesData[i]);
	fprintf(f, "%s    pages in file    :  %i\n",
	        actualPrefix, art->numPagesInLastFile);
	fprintf(f, "%s    particles in file:  %i\n",
	        actualPrefix, art->numParticlesInLastFile);
} /* art_prettyPrint */

/*--- Implementations of local functions --------------------------------*/
static char *
local_createFileNameHeader(const char *pathToFiles,
                           const char *fileNameSuffix)
{
	char *fileName;
	char *fileStem;

	fileStem = local_getFileStem(pathToFiles, "PMcrd");

	if (fileNameSuffix == ART_USE_DEFAULT_SUFFIX)
		fileName = xstrmerge(fileStem, ART_DEFAULT_SUFFIX_STRING);
	else
		fileName = xstrmerge(fileStem, fileNameSuffix);

	xfree(fileStem);

	return fileName;
}

static char **
local_createFileNamesData(const char *pathToFiles,
                          const char *fileNameSuffix,
                          int        numFiles)
{
	char **fileNames;
	char *fileStem;
	char *thisFileStem;
	int  digits;

	fileStem     = local_getFileStem(pathToFiles, "PMcrs");
	thisFileStem = xmalloc(sizeof(char) * strlen(fileStem)
	                       + ART_MAX_FILEDIGITS + 1);
	digits       = local_getRequiredDigits(numFiles);

	assert(numFiles > 0 && numFiles < ART_MAX_NUMFILES);
	fileNames = xmalloc(sizeof(char *) * numFiles);
	for (int i = 0; i < numFiles; i++) {
		sprintf(thisFileStem, "%s%0*i", fileStem, digits, i);
		if (fileNameSuffix == ART_USE_DEFAULT_SUFFIX)
			fileNames[i] = xstrmerge(thisFileStem,
			                         ART_DEFAULT_SUFFIX_STRING);
		else
			fileNames[i] = xstrmerge(thisFileStem, fileNameSuffix);
	}

	xfree(thisFileStem);
	xfree(fileStem);

	return fileNames;
}

static char *
local_getFileStem(const char *path, const char *fileNameStart)
{
	char *properPath;
	char *fileStem;

	if (path == ART_USE_DEFAULT_PATH)
		properPath = xstrdup(ART_DEFAULT_PATH_STRING);
	else
		properPath = local_getProperPath(path);

	fileStem = xstrmerge(properPath, fileNameStart);

	xfree(properPath);

	return fileStem;
}

static char *
local_getProperPath(const char *path)
{
	char   *properPath;
	size_t len = strlen(path);

	if (path[len - 1] != '/')
		properPath = xstrmerge(path, "/");
	else
		properPath = xstrdup(path);

	return properPath;
}

static void
local_updateNumbers(art_t art)
{
	uint64_t numParticles = artHeader_getNumParticlesTotal(art->header);
	int      nrowc        = artHeader_getNrowc(art->header);

	if (art->truncateNrowc && nrowc > ART_MAX_NROWC)
		nrowc = ART_MAX_NROWC;

	art->numParticlesInPage = nrowc * nrowc;

	art->numTotalPages      = (int)(numParticles
	                                / art->numParticlesInPage);
	if (numParticles % art->numParticlesInPage != 0)
		art->numTotalPages++;

	assert(art->numTotalPages >= art->numFiles);

	art->numParticlesInLastPage = (int)(numParticles
	                                    - (art->numParticlesInPage
	                                       * (art->numTotalPages - 1)));

	art->numPagesInFile     = art->numTotalPages / art->numFiles;
	art->numPagesInLastFile = art->numTotalPages
	                          - ((art->numFiles - 1)
	                             * art->numPagesInFile);

	art->numParticlesInFile     = art->numPagesInFile
	                              * art->numParticlesInPage;
	art->numParticlesInLastFile = (art->numPagesInLastFile - 1)
	                              * art->numParticlesInPage
	                              + art->numParticlesInLastPage;
}

static void
local_writeComponent(art_t    art,
                     uint64_t pSkip,
                     uint64_t pWrite,
                     stai_t   component,
                     bool     doByteswap)
{
	if ((component == NULL) || (pSkip == art->numParticlesInPage)) {
		xfseek(art->f, art->numParticlesInPage * sizeof(float), SEEK_CUR);
		return;
	}
	float *buffer;
	bool  bufferIsAllocated = false;

	if (stai_isLinear(component)
	    && (stai_getSizeOfElementInBytes(component) == sizeof(float))) {
		buffer = stai_getBase(component);
	} else {
		buffer            = xmalloc(sizeof(float) * pWrite);
		bufferIsAllocated = true;
		local_fillBufferFromStai(buffer, component, pWrite);
	}

	if (doByteswap) {
		for (int i = 0; i < pWrite; i++)
			byteswap(buffer + i, sizeof(float));
	}

	xfseek(art->f, (long)pSkip * sizeof(float), SEEK_CUR);
	xfwrite(buffer, sizeof(float), pWrite, art->f);
	xfseek(art->f,
	       (long)(art->numParticlesInPage - pSkip - pWrite) * sizeof(float),
	       SEEK_CUR);

	if (doByteswap && !bufferIsAllocated) {
		// Restore the original byte order of the data array.
		for (int i = 0; i < art->numParticlesInPage; i++)
			byteswap(buffer + i, sizeof(float));
	}
	if (bufferIsAllocated)
		xfree(buffer);
} /* local_writeComponent */

static void
local_readComponent(art_t    art,
                    uint64_t pSkip,
                    uint64_t pRead,
                    stai_t   component,
                    bool     doByteswap)
{
	if ((component == NULL) || (pSkip == art->numParticlesInPage)) {
		xfseek(art->f, art->numParticlesInPage * sizeof(float), SEEK_CUR);
		return;
	}
	float *buffer;
	bool  bufferIsAllocated;

	if (stai_isLinear(component)
	    && (stai_getSizeOfElementInBytes(component) == sizeof(float))) {
		buffer = stai_getBase(component);
	} else {
		buffer            = xmalloc(sizeof(float) * pRead);
		bufferIsAllocated = true;
	}

	xfseek(art->f, (long)pSkip * sizeof(float), SEEK_CUR);
	xfread(buffer, sizeof(float), pRead, art->f);
	xfseek(art->f,
	       (long)(art->numParticlesInPage - pSkip - pRead) * sizeof(float),
	       SEEK_CUR);
	if (doByteswap) {
		for (int i = 0; i < pRead; i++)
			byteswap(buffer + i, sizeof(float));
	}

	if (bufferIsAllocated) {
		local_copyBufferToStai(buffer, component, pRead);
		xfree(buffer);
	}
}

static void
local_fillBufferFromStai(float *buffer, stai_t stai, int numValues)
{
	if (stai_getSizeOfElementInBytes(stai) == sizeof(float)) {
		stai_getElementsMulti(stai, 0, buffer, numValues);
	} else if (stai_getSizeOfElementInBytes(stai) == sizeof(double)) {
		double d;
		for (int i = 0; i < numValues; i++) {
			stai_getElement(stai, i, &d);
			buffer[i] = (float)d;
		}
	} else {
		diediedie(EXIT_FAILURE);
	}
}

static void
local_copyBufferToStai(const float *buffer, stai_t stai, int numValues)
{
	if (stai_getSizeOfElementInBytes(stai) == sizeof(float)) {
		stai_setElementsMulti(stai, 0, buffer, numValues);
	} else if (stai_getSizeOfElementInBytes(stai) == sizeof(double)) {
		double d;
		for (int i = 0; i < numValues; i++) {
			d = (double)(buffer[i]);
			stai_setElement(stai, i, &d);
		}
	} else {
		diediedie(EXIT_FAILURE);
	}
}

static void
local_seekToPage(art_t art, int pageNumber)
{
	long pos;
	long posExpected;

	posExpected = ((long)art->numParticlesInPage) * ART_SIZEOF_PARTICLE
	              * pageNumber;
	pos         = ftell(art->f);

	if (pos != posExpected)
		xfseek(art->f, posExpected, SEEK_SET);
}

inline static void
local_rebaseStaisData(stai_t *data, int64_t offsetElements)
{
	if (data[0] != NULL)
		stai_rebase(data[0], offsetElements);
	if (data[1] != NULL)
		stai_rebase(data[1], offsetElements);
	if (data[2] != NULL)
		stai_rebase(data[2], offsetElements);
	if (data[3] != NULL)
		stai_rebase(data[3], offsetElements);
	if (data[4] != NULL)
		stai_rebase(data[4], offsetElements);
	if (data[5] != NULL)
		stai_rebase(data[5], offsetElements);
}

static void
local_calcFirstLast(int      normalizer,
                    int      largest,
                    uint64_t pSkip,
                    uint64_t pAct,
                    int      *first,
                    int      *last)
{
	*first = pSkip / normalizer;
	if (*first > largest)
		*first = largest;

	*last = (pSkip + pAct - 1) / normalizer;
	if (*last > largest)
		*last = largest;
}

static void
local_calcSkipAct(int      normalizer,
                  uint64_t pSkip,
                  uint64_t pAct,
                  uint64_t *pSkipCalc,
                  uint64_t *pActCalc)
{
	if (pSkip > normalizer)
		*pSkipCalc = normalizer;
	else
		*pSkipCalc = pSkip;

	if (pAct > normalizer - *pSkipCalc)
		*pActCalc = normalizer - *pSkipCalc;
	else
		*pActCalc = pAct;
}

static int
local_getRequiredDigits(int numFiles)
{
	int digits  = 1;
	int divisor = 10;

	while (digits < ART_MAX_FILEDIGITS && numFiles / divisor != 0) {
		digits++;
		divisor *= 10;
	}

	return digits;
}
