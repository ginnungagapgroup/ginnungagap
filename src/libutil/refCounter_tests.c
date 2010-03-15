// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "refCounter_tests.h"
#include "refCounter.h"
#include <stdio.h>
#include <string.h>
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
refCounter_init_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	refCounter_t refCounter;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	refCounter_init(&refCounter);
	if (refCounter != 0)
		hasPassed = false;
	refCounter_ref(&refCounter);
	refCounter_init(&refCounter);
	if (refCounter != 0)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
refCounter_ref_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	refCounter_t refCounter;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	refCounter_init(&refCounter);
	refCounter_ref(&refCounter);
	if (refCounter != 1)
		hasPassed = false;
	refCounter_ref(&refCounter);
	refCounter_ref(&refCounter);
	if (refCounter != 3)
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
refCounter_deref_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	refCounter_t refCounter;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	refCounter_init(&refCounter);
	refCounter_ref(&refCounter);
	refCounter_ref(&refCounter);
	refCounter_ref(&refCounter);
	if (refCounter_deref(&refCounter))
		hasPassed = false;
	if (refCounter_deref(&refCounter))
		hasPassed = false;
	if (!refCounter_deref(&refCounter))
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
refCounter_noReferenceLeft_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	refCounter_t refCounter;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	refCounter_init(&refCounter);
	refCounter_ref(&refCounter);
	refCounter_ref(&refCounter);
	if (refCounter_noReferenceLeft(refCounter))
		hasPassed = false;
	refCounter_deref(&refCounter);
	refCounter_deref(&refCounter);
	if (!refCounter_noReferenceLeft(refCounter))
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
