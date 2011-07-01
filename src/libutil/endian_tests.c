// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libutil/endian_tests.c
 * @ingroup  libutilMiscEndian
 * @brief  This provides the implementations of the test functions for
 *         endian.c.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "endian_tests.h"
#include "endian.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "xmem.h"
#endif
#include "xfile.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
endian_getFileEndianessByBlock_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (endian_getFileEndianessByBlock("tests/littleEndian.dat")
	    != ENDIAN_LITTLE)
		hasPassed = false;
	if (endian_getFileEndianessByBlock("tests/bigEndian.dat") != ENDIAN_BIG)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
endian_getFileEndianessByBlockF_test(void)
{
	bool   hasPassed = true;
	int    rank      = 0;
	FILE   *f;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	f = xfopen("tests/littleEndian.dat", "rb");
	if (endian_getFileEndianessByBlockF(f) != ENDIAN_LITTLE)
		hasPassed = false;
	xfclose(&f);

	f = xfopen("tests/bigEndian.dat", "rb");
	if (endian_getFileEndianessByBlockF(f) != ENDIAN_BIG)
		hasPassed = false;
	xfclose(&f);


#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
endian_fileIsLittleByBlock_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (!endian_fileIsLittleByBlock("tests/littleEndian.dat"))
		hasPassed = false;
	if (endian_fileIsLittleByBlock("tests/bigEndian.dat"))
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
endian_fileIsBigByBlock_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (!endian_fileIsBigByBlock("tests/bigEndian.dat"))
		hasPassed = false;
	if (endian_fileIsBigByBlock("tests/littleEndian.dat"))
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
endian_fileIsLittleByBlockF_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	FILE *f;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	f = xfopen("tests/littleEndian.dat", "rb");
	if (!endian_fileIsLittleByBlockF(f))
		hasPassed = false;
	xfclose(&f);

	f = xfopen("tests/bigEndian.dat", "rb");
	if (endian_fileIsLittleByBlockF(f))
		hasPassed = false;
	xfclose(&f);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
endian_fileIsBigByBlockF_test(void)
{
	bool   hasPassed      = true;
	int    rank           = 0;
	FILE *f;
#ifdef XMEM_TRACK_MEM
	size_t allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	f = xfopen("tests/littleEndian.dat", "rb");
	if (endian_fileIsBigByBlockF(f))
		hasPassed = false;
	xfclose(&f);

	f = xfopen("tests/bigEndian.dat", "rb");
	if (!endian_fileIsBigByBlockF(f))
		hasPassed = false;
	xfclose(&f);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
