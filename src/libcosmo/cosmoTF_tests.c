// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoTF_tests.h"
#include "cosmoTF.h"
#include <stdio.h>
#include <string.h>
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
cosmoTF_eisensteinHu1998_test(void)
{
	bool      hasPassed = true;
	//cosmoTF_t cosmoTF;
#ifdef XMEM_TRACK_MEM
	size_t    allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

	// INSERT TESTFUNCTION HERE
	printf("test not implemented, so: ");
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
