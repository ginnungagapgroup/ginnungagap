// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGET_ADT_H
#define GADGET_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadget_adt.h
 * @ingroup libutilFilesGadget
 * @brief  This file implements the main Gadget structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadget.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "gadgetVersion.h"
#include "gadgetHeader.h"
#include "gadgetTOC.h"
#include "../libutil/endian.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief The main Gadget structure holding all required information. */
struct gadget_struct {
	/** @brief The number of files comprising the snapshot. */
	int             numFiles;
	/** @brief Indicates the Gadget file version. */
	gadgetVersion_t fileVersion;
	/** @brief An array of length #numFiles holding the file names. */
	char            **fileNames;
	/** @brief Keeps track whether the values need to be byteswapped. */
	bool            doByteSwap;
	/** @brief The file pointer to the currently opened file in the set. */
	FILE            *f;
	/** @brief The mode in which the snapshot is opened. */
	gadget_mode_t   mode;
	/** @brief Keeps track of the last file that was opened. */
	int             lastOpened;
	/** @brief An array of length #numFiles holding the headers. */
	gadgetHeader_t  *headers;
	/** @brief An array of length #numFiles holding the TOC of each file. */
	gadgetTOC_t     *tocs;
};

#endif
