// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "refCounter.h"
#include <assert.h>
#include <stdbool.h>


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern void
refCounter_init(refCounter_t *refCount)
{
	*refCount = 0;
}

extern void
refCounter_ref(refCounter_t *curRef)
{
	*curRef += 1;
}

extern bool
refCounter_deref(refCounter_t *curRef)
{
	assert(curRef > 0);

	*curRef -= 1;

	return refCounter_noReferenceLeft(*curRef);
}

extern bool
refCounter_noReferenceLeft(refCounter_t curRef)
{
	if (curRef == 0)
		return true;

	return false;
}

/*--- Implementations of local functions --------------------------------*/
