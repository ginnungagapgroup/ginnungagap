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
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "gadget.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief The main Gadget structure holding all required information. */
struct gadget_struct {
	/** @brief The number of files comprising the snapshot. */
	int            numFiles;
	/** @brief An array of length #numFiles holding the file names. */
	char           **fileNames;
	/** @brief An array of length #numFiles holding the file pointer. */
	FILE           **f;
	/** @brief The mode in which the snapshot is opened. */
	gadgetMode_t   mode;
	/** @brief An array of length #numFiles holding the headers. */
	gadgetHeader_t *headers;
};

#endif
