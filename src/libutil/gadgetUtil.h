// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GADGETUTIL_H
#define GADGETUTIL_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetUtil.h
 * @ingroup libutilFilesGadget
 * @brief  This file provides the interface to utility functions dealing
 *         with the Gadget format.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>
#include <stdint.h>


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Writes a Gadget block description to the file.
 *
 * @param[in]  *f
 *                The file to which to write to.
 * @param[in]  *blockName
 *                The name of the block.  Must be an array of at least 4
 *                characters (only 4 will be written).
 * @param[in]  dataBlockSize
 *                The size of the data block.
 */
extern void
gadgetUtil_writeBlock(FILE       *f,
                      const char *blockName,
                      uint32_t   dataBlockSize);

#endif
