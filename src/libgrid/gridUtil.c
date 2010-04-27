// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridUtil.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/
#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridUtil_intersection1D(uint32_t loA,
                        uint32_t hiA,
                        uint32_t loB,
                        uint32_t hiB,
                        uint32_t *loC,
                        uint32_t *hiC)
{
	if (loA > hiB || hiA < loB)
		return false;

	*loC = (loA < loB) ? MIN(hiA, loB) : loA;
	*hiC = (hiA > hiB) ? MAX(loA, hiB) : hiA;

	return true;
}

/*--- Implementations of local functions --------------------------------*/
