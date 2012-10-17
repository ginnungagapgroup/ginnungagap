// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETTOC_ADT_H
#define GADGETTOC_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetTOC_adt.h
 * @ingroup libutilFilesGadgetTOC
 * @brief  Provides the structure of the Table of Content ADT.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include "gadgetBlock.h"


/*--- Helper structures -------------------------------------------------*/

/**
 * @brief Helper structure to store the information of one block.
 *
 * Note that the offset gives the position in the file at which the leading
 * block delimiters are stored, i.e. every element in a Gadget file is
 * stored as @c uint32_t @c actualData @c uint32_t, the offset is hence the
 * offset to the first @c uint32_t.
 */
struct gadgetTOC_entry_struct {
	/** @brief  The offset of the descriptor block in bytes. */
	long          offset;
	/** @brief  The bock type. */
	gadgetBlock_t type;
	/** @brief  The size of the block in bytes. */
	uint32_t      sizeInBytes;
	/** @brief  The name of the block in Gadget V2 files. */
	char          nameInV2Files[5];
};


/*--- ADT implementation ------------------------------------------------*/

/**
 * @brief  The main table of content structure.
 */
struct gadgetTOC_struct {
	/** @brief  The total number of blocks in the file. */
	int numBlocks;

	/**
	 * @brief  Saves the file version (required to distinguish between data
	 *         block offsets and descriptor block offsets).
	 */
	gadgetVersion_t fileVersion;

	/** @brief  The actual blocks. */
	struct gadgetTOC_entry_struct *blocks;
};


#endif
