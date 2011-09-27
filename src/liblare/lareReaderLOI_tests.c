// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReaderLOI_tests.h"
#include "lareReaderLOI.h"
#include <stdio.h>
#include <string.h>
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implemention of main structure ------------------------------------*/
#include "lareReaderLOI_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
lareReaderLOI_new_test(void)
{
	bool            hasPassed = true;
	lareReaderLOI_t lareReaderLOI;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
lareReaderLOI_del_test(void)
{
	bool            hasPassed = true;
	lareReaderLOI_t lareReaderLOI;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
lareReaderLOI_read_test(void)
{
	bool            hasPassed = true;
	lareReaderLOI_t lareReaderLOI;
#ifdef XMEM_TRACK_MEM
	size_t          allocatedBytes = global_allocated_bytes;
#endif

	printf("Testing %s... ", __func__);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
