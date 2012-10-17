// Copyright (C) 2011, 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetBlock.c
 * @ingroup libutilFilesGadgetBlock
 * @brief  This file provides the implementions of the Gadget file block
 *         interface.
 */

/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetBlock.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "gadgetVersion.h"
#include "../libutil/xfile.h"
#include "../libutil/byteswap.h"


/*--- Local variables ---------------------------------------------------*/

/** @brief  Stores the size of the block description in bytes. */
static const uint32_t local_blockSize = 8;

/** @brief  Stores the size of the block description in bytes. (swapped). */
static const uint32_t local_blockSizeSwapped = 134217728;

/** @brief The names of the Gadget IO blocks. */
static const char *local_blockNames[GADGETBLOCK_NUM]
    = {"HEAD", "POS ", "VEL ", "ID  ", "MASS", "U   ", "RHO ", "NE  ",
	   "NH  ", "HSML", "SFR ", "AGE ", "Z   ", "Unknown"};

/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Skips a block of a given size.
 *
 * This assumes that the block exists and can be skipped, otherwise the
 * program will terminated with an IO error.
 *
 * @param[in,out]  *f
 *                    The file pointer, must be positioned at the
 *                    beginning of the block (i.e. at the first block
 *                    size integer).
 * @param[in]      blockSize
 *                    The size of the data block that should be skipped.
 *                    This should be size of the actual data without the
 *                    bracketing block size integers.
 *
 * @return  Returns nothing.
 */
inline static void
local_skipBlockByBlockSize(FILE *f, uint32_t blockSize);


/**
 * @brief  Skips over all block before a requested block.
 *
 * This only works for file version 1, the function only jumps over the
 * data blocks and requires them to be in the same order as they are
 * listed in #local_blockNames.
 *
 * @param[in,out]  *f
 *                    The file pointer to work with.
 * @param[in]      block
 *                    The block that should be reached.
 * @param[in]      doByteSwap
 *                    Flags whether the values read from the file must
 *                    be swapped to account for different endianess
 *                    between the system and the file.
 *
 * @return  Returns @c true if all preceding block could be skipped and
 *          @c false if at least one of them did not exist.
 */
inline static bool
local_skipOverPreceedingBlocks(FILE          *f,
                               gadgetBlock_t block,
                               bool          doByteSwap);


/**
 * @brief  Checks whether the file is positioned at the beginning of
 *         a block.
 *
 * @param[in,out]  *f
 *                    The file pointer to work with.
 *
 * @return  Returns @c true if the next block exists and @c false
 *          otherwise.
 */
inline static bool
local_checkIfNextBlockExists(FILE *f);


/**
 * @brief  Locates a block by parsing all block descriptors from the
 *         current position in the file.
 *
 * @param[in,out]  *f
 *                    The file pointer to work with.  Must be positioned
 *                    at the beginning of a descriptor block.
 * @param[in]      block
 *                    The block to search for.
 * @param[in]      doByteSwap
 *                    Flags if an endian correction is required
 *                    (@c true) or not (@c false);
 * @param[in]      version
 *                    The version of the Gadget file that is being read.
 *                    Must not be #GADGETVERSION_ONE.
 *
 * @return  Returns @c true if the block could be found and @c false
 *          otherwise.
 */
inline static bool
local_findBlockV2(FILE            *f,
                  gadgetBlock_t   block,
                  bool            doByteSwap,
                  gadgetVersion_t version);


/*--- Implementations of exported functios ------------------------------*/
extern const char *
gadgetBlock_getNameFromType(gadgetBlock_t blockType)
{
	return local_blockNames[blockType];
}

extern gadgetBlock_t
gadgetBlock_getTypeFromName(const char *name)
{
	int           i   = 0;
	gadgetBlock_t rtn = GADGETBLOCK_UNKNOWN;

	while (i < GADGETBLOCK_NUM) {
		if (strncmp(name, local_blockNames[i], 4) == 0) {
			rtn = (gadgetBlock_t)i;
			break;
		}
		i++;
	}

	return rtn;
}

extern uint32_t
gadgetBlock_getNumComponents(const gadgetBlock_t block)
{
	assert(block != GADGETBLOCK_UNKNOWN);
	assert(block != GADGETBLOCK_HEAD);

	uint32_t numComponents;

	switch (block) {
	case GADGETBLOCK_POS_:
	case GADGETBLOCK_VEL_:
		numComponents = 3;
		break;
	default:
		numComponents = 1;
	}

	return numComponents;
}

extern bool
gadgetBlock_isInteger(const gadgetBlock_t block)
{
	if (block == GADGETBLOCK_ID__)
		return true;

	return false;
}

extern uint32_t
gadgetBlock_getNumPartsInBlock(const gadgetBlock_t block,
                               const uint32_t      np[6],
                               const double        massarr[6])
{
	uint32_t numParts = UINT32_C(0);

	switch (block) {
	case GADGETBLOCK_POS_:
	case GADGETBLOCK_VEL_:
	case GADGETBLOCK_ID__:
		for (int i = 0; i < 6; i++)
			numParts += np[i];
		break;
	case GADGETBLOCK_MASS:
		for (int i = 0; i < 6; i++)
			if (!isgreater(massarr[i], 0.0))
				numParts += np[i];
		break;
	case GADGETBLOCK_U___:
	case GADGETBLOCK_RHO_:
	case GADGETBLOCK_NE__:
	case GADGETBLOCK_NH__:
	case GADGETBLOCK_HSML:
	case GADGETBLOCK_SFR_:
		numParts = np[0];
		break;
	case GADGETBLOCK_AGE_:
		numParts = np[4];
		break;
	case GADGETBLOCK_Z___:
		numParts = np[0] + np[4];
		break;
	default:
		break;
	}

	return numParts;
} /* gadgetBlock_getNumPartsInBlock */

extern void
gadgetBlock_writeDescriptor(FILE            *f,
                            gadgetBlock_t   block,
                            uint32_t        dataBlockSize,
                            bool            doByteSwap,
                            gadgetVersion_t fileVersion)
{
	assert(f != NULL);
	assert(block != GADGETBLOCK_UNKNOWN);

	if (fileVersion != GADGETVERSION_ONE) {
		uint32_t nextBlockSize = dataBlockSize + 8; // Fortran delimiters!

		xfwrite(doByteSwap ? &local_blockSizeSwapped : &local_blockSize,
		        sizeof(uint32_t), 1, f);

		xfwrite(gadgetBlock_getNameFromType(block), sizeof(char), 4, f);
		if (doByteSwap)
			byteswap(&nextBlockSize, sizeof(uint32_t));
		xfwrite(&nextBlockSize, sizeof(uint32_t), 1, f);

		xfwrite(doByteSwap ? &local_blockSizeSwapped : &local_blockSize,
		        sizeof(uint32_t), 1, f);
	}
}

extern bool
gadgetBlock_readDescriptor(FILE            *f,
                           gadgetBlock_t   *block,
                           uint32_t        *dataBlockSize,
                           bool            doByteSwap,
                           gadgetVersion_t fileVersion)
{
	assert(f != NULL);

	bool blockRead = false;
	if (fileVersion != GADGETVERSION_ONE) {
		char blockname[4];
		blockRead = gadgetBlock_readDescriptorString(f, blockname,
		                                             dataBlockSize,
		                                             doByteSwap,
		                                             fileVersion);
		if (blockRead && (block != NULL))
			*block = gadgetBlock_getTypeFromName(blockname);
	}

	return blockRead ? true : false;
}

extern bool
gadgetBlock_readDescriptorString(FILE            *f,
                                 char            *name,
                                 uint32_t        *dataBlockSize,
                                 bool            doByteSwap,
                                 gadgetVersion_t fileVersion)
{
	assert(f != NULL);

	bool blockRead = false;

	if (fileVersion != GADGETVERSION_ONE) {
		uint32_t bs1, bs2, nextBlockSizeFile;

		if (fread(&bs1, sizeof(uint32_t), 1, f) == 1) {
			xfread(name, sizeof(char), 4, f);
			xfread(&nextBlockSizeFile, sizeof(uint32_t), 1, f);
			xfread(&bs2, sizeof(uint32_t), 1, f);
			if (doByteSwap) {
				byteswap(&bs1, sizeof(uint32_t));
				byteswap(&nextBlockSizeFile, sizeof(uint32_t));
				byteswap(&bs2, sizeof(uint32_t));
			}

			if ((bs1 == bs2) && (bs1 == local_blockSize)) {
				if (dataBlockSize != NULL)
					*dataBlockSize = nextBlockSizeFile - 8;  // Get real size.
				blockRead = true;
			} else {
				xfseek(f, -16L, SEEK_CUR); // Wasn't a descriptor block.
			}
		}
	}

	return blockRead ? true : false;
}

extern bool
gadgetBlock_find(FILE            *f,
                 gadgetBlock_t   block,
                 bool            doByteSwap,
                 gadgetVersion_t version)
{
	assert(f != NULL);
	assert(block != GADGETBLOCK_UNKNOWN);

	bool found = false;

	rewind(f);

	if (version == GADGETVERSION_ONE) {
		if (local_skipOverPreceedingBlocks(f, block, doByteSwap))
			found = local_checkIfNextBlockExists(f);
	} else {
		found = local_findBlockV2(f, block, doByteSwap, version);
	}

	return found ? true : false;
}

extern bool
gadgetBlock_skip(FILE *f, bool doByteSwap)
{
	return gadgetBlock_skip2(f, doByteSwap, NULL);
}

extern bool
gadgetBlock_skip2(FILE *f, bool doByteSwap, uint32_t *blockSizeInBytes)
{
	assert(f != NULL);

	uint32_t bs1, bs2;
	long     oldPos = xftell(f);
	bool     failed = false;

	if (fread(&bs1, sizeof(uint32_t), 1, f) != 1) {
		failed = true;
	} else {
		if (doByteSwap)
			byteswap(&bs1, sizeof(uint32_t));
		if (fseek(f, (long)bs1, SEEK_CUR) != 0)
			failed = true;
		else if (fread(&bs2, sizeof(uint32_t), 1, f) != 1)
			failed = true;
		else {
			if (doByteSwap)
				byteswap(&bs2, sizeof(uint32_t));
			if (bs1 != bs2)
				failed = true;
		}
	}
	if (failed) {
		if (ferror(f))
			clearerr(f);
		xfseek(f, oldPos, SEEK_SET);
	} else if (blockSizeInBytes != NULL) {
		*blockSizeInBytes = bs1;
	}

	return failed ? false : true;
} /* gadgetBlock_skip2 */

extern void
gadgetBlock_writeEmptyBlockToFile(FILE     *f,
                                  uint32_t blockSize,
                                  bool     doByteSwap)
{
	assert(f != NULL);
	assert(blockSize != 0);

	const char zero = 0;

	gadgetBlock_writeBlockSize(f, blockSize, doByteSwap);
	for (uint32_t i = 0; i < blockSize; i++) {
		xfwrite(&zero, sizeof(char), 1, f);
	}
	gadgetBlock_writeBlockSize(f, blockSize, doByteSwap);
}

extern void
gadgetBlock_writeBlockSize(FILE *f, uint32_t blockSize, bool doByteSwap)
{
	if (doByteSwap)
		byteswap(&blockSize, sizeof(uint32_t));

	xfwrite(&blockSize, sizeof(uint32_t), 1, f);
}

extern void
gadgetBlock_readBlockSize(FILE *f, uint32_t *blockSize, bool doByteSwap)
{
	xfread(blockSize, sizeof(uint32_t), 1, f);

	if (doByteSwap)
		byteswap(blockSize, sizeof(uint32_t));
}

/*--- Implementations of local functions --------------------------------*/
inline static void
local_skipBlockByBlockSize(FILE *f, uint32_t blockSize)
{
	xfseek(f, (long)blockSize + 4L, SEEK_CUR);
}

inline static bool
local_skipOverPreceedingBlocks(FILE          *f,
                               gadgetBlock_t block,
                               bool          doByteSwap)
{
	gadgetBlock_t nextBlock = (gadgetBlock_t)0;
	bool          couldSkip = true;

	while (couldSkip && nextBlock != block) {
		couldSkip = gadgetBlock_skip(f, doByteSwap);
		nextBlock = (gadgetBlock_t)((int)nextBlock + 1);
	}

	return nextBlock == block ? true : false;
}

inline static bool
local_checkIfNextBlockExists(FILE *f)
{
	uint32_t tmp;
	bool     found = false;

	if (fread(&tmp, sizeof(uint32_t), 1, f) == 1) {
		xfseek(f, -(long)(sizeof(uint32_t)), SEEK_CUR);
		found = true;
	}

	return found ? true : false;
}

inline static bool
local_findBlockV2(FILE            *f,
                  gadgetBlock_t   block,
                  bool            doByteSwap,
                  gadgetVersion_t version)
{
	gadgetBlock_t nextBlock;
	uint32_t      nextBlockSize;
	bool          hasNextBlock, found = false;

	hasNextBlock = gadgetBlock_readDescriptor(f, &nextBlock,
	                                          &nextBlockSize,
	                                          doByteSwap, version);

	while (hasNextBlock && nextBlock != block) {
		local_skipBlockByBlockSize(f, nextBlockSize);
		hasNextBlock = gadgetBlock_readDescriptor(f, &nextBlock,
		                                          &nextBlockSize,
		                                          doByteSwap, version);
	}

	if (hasNextBlock && (nextBlock == block))
		found = true;

	return found ? true : false;
}
