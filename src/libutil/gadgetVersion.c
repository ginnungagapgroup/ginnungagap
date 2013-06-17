// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/gadgetVersion.c
 * @ingroup libutilFilesGadgetVersion
 * @brief  This file implements functionality to deal with Gadget file
 *         versions.
 */

/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "gadgetVersion.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../libutil/xfile.h"
#include "gadgetHeader.h"


/*--- Local variables ---------------------------------------------------
** @brief The names of the Gadget IO blocks. */
static const char *local_versionNames[GADGETVERSION_NUM]
    = {"Unknown",
	   "v1 (no block descriptors)",
	   "v2 (with block descriptors)"};


/*--- Implementations of exported functios ------------------------------*/
extern gadgetVersion_t
gadgetVersion_getVersionFromFile(FILE *f)
{
	assert(f != NULL);

	gadgetVersion_t version;
	long            restorePos = xftell(f);
	char            buffer[4 + GADGETHEADER_SIZE + 4];
	int             bs1, bs2, bs3;

	xfread(buffer, 1, 4 + GADGETHEADER_SIZE + 4, f);

	memcpy( &bs1, buffer, sizeof(int) );
	memcpy( &bs2, buffer + 12, sizeof(int) );
	memcpy( &bs3, buffer + 4 + GADGETHEADER_SIZE, sizeof(int) );

	if ( (bs1 == bs2)
	     && (strncmp(buffer + 4, "HEAD", 4) == 0) ) {
		version = GADGETVERSION_TWO;
	} else if (bs1 == bs3) {
		version = GADGETVERSION_ONE;
	} else {
		version = GADGETVERSION_UNKNOWN;
	}

	xfseek(f, restorePos, SEEK_SET);

	return version;
} // gadgetVersion_getVersionFromFile

extern const char *
gadgetVersion_getNameFromType(const gadgetVersion_t ver)
{
	return local_versionNames[ver];
}

extern gadgetVersion_t
gadgetVersion_getTypeFromName(const char *name)
{
	for (int i = 0; i < GADGETVERSION_NUM; i++) {
		const int cmp = strncmp( name, local_versionNames[i],
		                         strlen(local_versionNames[i]) );
		if (cmp == 0)
			return i;
	}
	return GADGETVERSION_UNKNOWN;
}
