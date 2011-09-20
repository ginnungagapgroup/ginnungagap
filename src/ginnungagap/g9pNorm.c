// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file g9pNorm.c
 * @ingroup  ginnungagapNorm
 * @brief  Provides the implementation of the normaliztion modes.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pNorm.h"
#include <string.h>
#include <assert.h>


/*--- Local defines -----------------------------------------------------*/

/** @brief  Defines how many modes are available. */
#define LOCAL_NUM_MODES 4


/*--- Local variables ---------------------------------------------------*/

/** @brief  Provides the names of the modes. */
static const char *const local_modeStr[LOCAL_NUM_MODES]
    = { "none", "sigma8", "sigma8Box", "unknown" };


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern g9pNorm_mode_t
g9pNorm_getModeFromName(const char *name)
{
	g9pNorm_mode_t mode = G9PNORM_MODE_UNKNOWN;

	assert(name != NULL);

	for (int i = 0; i < LOCAL_NUM_MODES; i++) {
		if ((strlen(name) == strlen(local_modeStr[i]))
		    && (strcmp(name, local_modeStr[i]) == 0)) {
			mode = (g9pNorm_mode_t)i;
			break;
		}
	}

	return mode;
}

extern const char *
g9pNorm_getNameFromMode(g9pNorm_mode_t mode)
{
	return local_modeStr[mode];
}

/*--- Implementations of local functions --------------------------------*/
