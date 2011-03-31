// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetUtil.c
 * @ingroup libutilFilesGadget
 * @brief  This file provides the implementions of the utility functions
 *         dealing with the Gadget format.
 */

/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetUtil.h"
#include <assert.h>
#include <stdint.h>
#include "../libutil/xfile.h"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern void
gadgetUtil_writeBlock(FILE       *f,
                      const char *blockName,
                      uint32_t   dataBlockSize)
{
	static const uint32_t thisBlockSize = 8;
	uint32_t              nextBlockSize = dataBlockSize + 8;

	assert(f != NULL);
	assert(blockName != NULL);

	xfwrite(&thisBlockSize, sizeof(uint32_t), 1, f);

	xfwrite(blockName, sizeof(char), 4, f);
	xfwrite(&nextBlockSize, sizeof(uint32_t), 1, f);

	xfwrite(&thisBlockSize, sizeof(uint32_t), 1, f);
}

/*--- Implementations of local functions --------------------------------*/
