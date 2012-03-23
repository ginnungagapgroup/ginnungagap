// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetTOC.c
 * @ingroup libutilFilesGadgetTOC
 * @brief  Implements the TOC object for Gadget files.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetTOC.h"
#include "gadgetHeader.h"
#include <assert.h>
#include <inttypes.h>
#include <string.h>
#include "../libutil/xmem.h"
#include "../libutil/xfile.h"
#include "../libutil/endian.h"


/*--- Implementation of main structure ----------------------------------*/
#include "gadgetTOC_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/

/**
 * @brief  Adds a new element to the toc.
 *
 * @param[in,out]  toc
 *                    The TOC to which to add the entry.
 * @param[in]      *entry
 *                    The entry that should be added.
 *
 * @return  Returns nothing.
 */
inline static void
local_addElement(gadgetTOC_t                         toc,
                 const struct gadgetTOC_entry_struct *entry);


/**
 * @brief  Helper function implementing the construction of a TOC from a
 *         file.
 *
 * @param[in,out]  toc
 *                    The TOC to construct.
 * @param[in,out]  *f
 *                    The file to work with.
 * @param[in]      doByteSwap
 *                    Flags whether endianess correction needs to be done.
 * @param[in]      version
 *                    The version of the Gadget file.
 *
 * @return  Returns nothing.
 */
inline static void
local_newFromFileActual(gadgetTOC_t     toc,
                        FILE            *f,
                        bool            doByteSwap,
                        gadgetVersion_t version);


/*--- Implementations of exported functions -----------------------------*/
extern gadgetTOC_t
gadgetTOC_new(void)
{
	gadgetTOC_t toc;

	toc              = xmalloc(sizeof(struct gadgetTOC_struct));
	toc->fileVersion = GADGETVERSION_TWO;
	toc->numBlocks   = 0;
	toc->blocks      = NULL;

	return toc;
}

extern gadgetTOC_t
gadgetTOC_newFromFile(FILE *f)
{
	assert(f != NULL);

	gadgetTOC_t toc        = gadgetTOC_new();
	long        restorePos = ftell(f);
	bool        doByteSwap;

	rewind(f);

	toc->fileVersion = gadgetVersion_getVersionFromFile(f);
	if (toc->fileVersion == GADGETVERSION_UNKNOWN)
		return toc;

	doByteSwap = (endian_fileIsLittleByBlockF(f) != endian_systemIsLittle());

	local_newFromFileActual(toc, f, doByteSwap, toc->fileVersion);

	if (ferror(f))
		clearerr(f);
	fseek(f, restorePos, SEEK_SET);

	return toc;
}

extern gadgetTOC_t
gadgetTOC_clone(const gadgetTOC_t tmpl)
{
	assert(tmpl != NULL);

	gadgetTOC_t clone;
	size_t      sizeBlocks = sizeof(struct gadgetTOC_entry_struct)
	                         * tmpl->numBlocks;

	clone              = xmalloc(sizeof(struct gadgetTOC_struct));
	clone->fileVersion = tmpl->fileVersion;
	clone->numBlocks   = tmpl->numBlocks;
	clone->blocks      = xmalloc(sizeBlocks);
	memcpy(clone->blocks, tmpl->blocks, sizeBlocks);

	return clone;
}

extern void
gadgetTOC_del(gadgetTOC_t *toc)
{
	assert(toc != NULL && *toc != NULL);

	if ((*toc)->blocks != NULL)
		xfree((*toc)->blocks);
	xfree(*toc);

	*toc = NULL;
}

extern void
gadgetTOC_setFileVersion(gadgetTOC_t toc, gadgetVersion_t ver)
{
	assert(toc != NULL);
	assert(ver != GADGETVERSION_UNKNOWN);

	if (toc->fileVersion != ver) {
		toc->fileVersion = ver;
		gadgetTOC_calcOffset(toc);
	}
}

extern gadgetVersion_t
gadgetTOC_getFileVersion(const gadgetTOC_t toc)
{
	assert(toc != NULL);

	return toc->fileVersion;
}

extern bool
gadgetTOC_blockExists(const gadgetTOC_t toc, gadgetBlock_t block)
{
	assert(toc != NULL);

	for (int i = 0; i < toc->numBlocks; i++) {
		if (toc->blocks[i].type == block)
			return true;
	}

	return false;
}

extern bool
gadgetTOC_blockExistsByName(const gadgetTOC_t toc, const char *name)
{
	assert(toc != NULL);
	assert(name != NULL);

	for (int i = 0; i < toc->numBlocks; i++) {
		if (strncmp(toc->blocks[i].nameInV2Files, name, 4) == 0)
			return true;
	}

	return false;
}

extern long
gadgetTOC_getOffsetForBlock(const gadgetTOC_t toc, gadgetBlock_t block)
{
	for (int i = 0; i < toc->numBlocks; i++) {
		if (toc->blocks[i].type == block)
			return toc->blocks[i].offset;
	}
	return -1L;
}

extern uint32_t
gadgetTOC_getSizeInBytesForBlock(const gadgetTOC_t toc, gadgetBlock_t block)
{
	for (int i = 0; i < toc->numBlocks; i++) {
		if (toc->blocks[i].type == block)
			return toc->blocks[i].sizeInBytes;
	}
	return UINT32_C(0);
}

extern const char *
gadgetTOC_getNameInV2FilesForBlock(const gadgetTOC_t toc,
                                   gadgetBlock_t     block)
{
	for (int i = 0; i < toc->numBlocks; i++) {
		if (toc->blocks[i].type == block)
			return toc->blocks[i].nameInV2Files;
	}
	return NULL;
}

extern long
gadgetTOC_getOffsetBySeqNumber(const gadgetTOC_t toc, int seqNumber)
{
	assert(toc != NULL);
	assert(seqNumber >= 0 && seqNumber < toc->numBlocks);

	return toc->blocks[seqNumber].offset;
}

extern gadgetBlock_t
gadgetTOC_getTypeBySeqNumber(const gadgetTOC_t toc, int seqNumber)
{
	assert(toc != NULL);
	assert(seqNumber >= 0 && seqNumber < toc->numBlocks);

	return toc->blocks[seqNumber].type;
}

extern uint32_t
gadgetTOC_getSizeInBytesBySeqNumber(const gadgetTOC_t toc, int seqNumber)
{
	assert(toc != NULL);
	assert(seqNumber >= 0 && seqNumber < toc->numBlocks);

	return toc->blocks[seqNumber].sizeInBytes;
}

extern const char *
gadgetTOC_getNameBySeqNumber(const gadgetTOC_t toc, int seqNumber)
{
	assert(toc != NULL);
	assert(seqNumber >= 0 && seqNumber < toc->numBlocks);

	return toc->blocks[seqNumber].nameInV2Files;
}

extern size_t
gadgetTOC_getTotalFileSize(const gadgetTOC_t toc)
{
	assert(toc != NULL);

	return toc->blocks[toc->numBlocks - 1].offset
	       + gadgetTOC_getSizeInBytesBySeqNumber(toc, toc->numBlocks - 1);
}

extern void
gadgetTOC_addEntryByName(gadgetTOC_t toc, const char *name)
{
	assert(toc != NULL);
	assert(name != NULL);

	if (!gadgetTOC_blockExistsByName(toc, name)) {
		struct gadgetTOC_entry_struct entry;

		entry.offset           = -1;
		entry.type             = gadgetBlock_getTypeFromName(name);
		entry.sizeInBytes      = UINT32_C(0);
		memcpy(entry.nameInV2Files, name, 4);
		entry.nameInV2Files[4] = '\0';

		local_addElement(toc, &entry);
	}
}

extern void
gadgetTOC_addEntryByType(gadgetTOC_t toc, gadgetBlock_t type)
{
	assert(toc != NULL);
	assert(type != GADGETBLOCK_UNKNOWN);

	if (!gadgetTOC_blockExists(toc, type)) {
		struct gadgetTOC_entry_struct entry;

		entry.offset           = -1;
		entry.type             = type;
		entry.sizeInBytes      = UINT32_C(0);
		memcpy(entry.nameInV2Files, gadgetBlock_getNameFromType(type), 4);
		entry.nameInV2Files[4] = '\0';

		local_addElement(toc, &entry);
	}
}

extern void
gadgetTOC_calcSizes(gadgetTOC_t    toc,
                    const uint32_t np[6],
                    const double   massarr[6],
                    bool           useDoublePrec,
                    bool           useLongIDs)
{
	for (int i = 0; i < toc->numBlocks; i++) {
		gadgetBlock_t type = toc->blocks[i].type;
		if (type == GADGETBLOCK_HEAD) {
			toc->blocks[i].sizeInBytes = GADGETHEADER_SIZE;
		} else {
			uint32_t numParticles, sizePerParticle;
			numParticles    = gadgetBlock_getNumPartsInBlock(type,
			                                                 np,
			                                                 massarr);
			sizePerParticle = gadgetBlock_getNumComponents(type);
			if (type == GADGETBLOCK_ID__)
				sizePerParticle *= useLongIDs ? 8 : 4;
			else
				sizePerParticle *= useDoublePrec ? 8 : 4;
			toc->blocks[i].sizeInBytes = numParticles * sizePerParticle;
		}
	}
}

extern void
gadgetTOC_calcOffset(gadgetTOC_t toc)
{
	assert(toc != NULL);

	long curOffset = 0;

	for (int i = 0; i < toc->numBlocks; i++) {
		if (toc->fileVersion != GADGETVERSION_ONE)
			curOffset += 4 + GADGETBLOCK_DESCRIPTOR_BLOCK_LENGTH  + 4;
		toc->blocks[i].offset = curOffset;
		curOffset            += 4 + toc->blocks[i].sizeInBytes + 4;
	}
}

extern void
gadgetTOC_seekToData(const gadgetTOC_t toc, gadgetBlock_t block, FILE *f)
{
	long offsetData = gadgetTOC_getOffsetForBlock(toc, block);

	xfseek(f, offsetData, SEEK_SET);
}

extern void
gadgetTOC_seekToDescriptor(const gadgetTOC_t toc,
                           gadgetBlock_t     block,
                           FILE              *f)
{
	long offsetDescriptor = gadgetTOC_getOffsetForBlock(toc, block);

	if (toc->fileVersion == GADGETVERSION_TWO) {
		assert(
		    offsetDescriptor >= 4 + GADGETBLOCK_DESCRIPTOR_BLOCK_LENGTH + 4);
		offsetDescriptor -= (4 + GADGETBLOCK_DESCRIPTOR_BLOCK_LENGTH + 4);
	}

	xfseek(f, offsetDescriptor, SEEK_SET);
}

extern bool
gadgetTOC_isValid(const gadgetTOC_t toc)
{
	long expectedMinOffset = 0;

	for (int i = 0; i < toc->numBlocks; i++) {
		if (toc->blocks[i].offset < expectedMinOffset)
			return false;

		expectedMinOffset = toc->blocks[i].offset
		                    + toc->blocks[i].sizeInBytes + 8;
	}

	return true;
}

extern void
gadgetTOC_prettyPrint(const gadgetTOC_t toc, FILE *out, const char *prefix)
{
	assert(toc != NULL);
	assert(out != NULL);

	const char *actualPrefix = prefix == NULL ? "" : prefix;

	fprintf(out, "%sTable of Content (%i entries)\n",
	        actualPrefix, toc->numBlocks);
	for (int i = 0; i < toc->numBlocks; i++)
		fprintf(out, "%s\t%i  Offset: %li  type: %s  size: %" PRIu32
		        "b  name: %s\n",
		        actualPrefix, i,
		        toc->blocks[i].offset,
		        gadgetBlock_getNameFromType(toc->blocks[i].type),
		        toc->blocks[i].sizeInBytes,
		        toc->blocks[i].nameInV2Files);
}

/*--- Implementations of local functions --------------------------------*/
inline static void
local_addElement(gadgetTOC_t                         toc,
                 const struct gadgetTOC_entry_struct *entry)
{
	toc->blocks = xrealloc(toc->blocks,
	                       sizeof(struct gadgetTOC_entry_struct)
	                       * (toc->numBlocks + 1));
	memcpy(toc->blocks + toc->numBlocks, entry,
	       sizeof(struct gadgetTOC_entry_struct));
	toc->numBlocks++;
}

inline static void
local_newFromFileActual(gadgetTOC_t     toc,
                        FILE            *f,
                        bool            doByteSwap,
                        gadgetVersion_t version)
{
	struct gadgetTOC_entry_struct entry;
	while (true) {
		if (version == GADGETVERSION_TWO) {
			if (!gadgetBlock_readDescriptorString(f, entry.nameInV2Files,
			                                      &(entry).sizeInBytes,
			                                      doByteSwap, version))
				break;
			entry.offset = xftell(f);
			entry.type   = gadgetBlock_getTypeFromName(entry.nameInV2Files);
			if (!gadgetBlock_skip(f, doByteSwap))
				break;
		} else {
			entry.offset = xftell(f);
			if (!gadgetBlock_skip2(f, doByteSwap, &(entry.sizeInBytes)))
				break;
			entry.type = (gadgetBlock_t)(toc->numBlocks);
			memcpy(entry.nameInV2Files,
			       gadgetBlock_getNameFromType(entry.type),
			       4);
		}
		entry.nameInV2Files[4] = '\0';
		local_addElement(toc, &entry);
	}
}
