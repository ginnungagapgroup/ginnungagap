// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/filename_tests.c
 * @ingroup  libutilMiscFilenameTest
 * @brief  Implements the test for the filename object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "filename_tests.h"
#include "filename.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Implementation of main structure ----------------------------------*/
#include "filename_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
filename_new_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_new();
	if (strcmp(fn->path, "") != 0)
		hasPassed = false;
	if (strcmp(fn->prefix, "") != 0)
		hasPassed = false;
	if (strcmp(fn->qualifier, "") != 0)
		hasPassed = false;
	if (strcmp(fn->suffix, "") != 0)
		hasPassed = false;
	filename_del(&fn);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
filename_newFull_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_newFull("<path>", "<prefix>", "<qualifier>", "<suffix>");
	if (strcmp(fn->fullName, "<path>/<prefix><qualifier><suffix>") != 0)
		hasPassed = false;
	filename_del(&fn);

	fn = filename_newFull("<path>/", "<prefix>", "<qualifier>", "<suffix>");
	if (strcmp(fn->fullName, "<path>/<prefix><qualifier><suffix>") != 0)
		hasPassed = false;
	filename_del(&fn);

	fn = filename_newFull(NULL, "<prefix>", "<qualifier>", "<suffix>");
	if (strcmp(fn->fullName, "<prefix><qualifier><suffix>") != 0)
		hasPassed = false;
	filename_del(&fn);

	fn = filename_newFull("<path>", NULL, "<qualifier>", NULL);
	if (strcmp(fn->fullName, "<path>/<qualifier>") != 0)
		hasPassed = false;
	filename_del(&fn);


#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* filename_newFull_test */

extern bool
filename_del_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_newFull("<path>", "<prefix>", "<qualifier>", "<suffix>");
	filename_del(&fn);
	if (fn != NULL)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
filename_setPath_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_new();

	filename_setPath(fn, "/usr/local/asd");
	if (strcmp(fn->fullName, "/usr/local/asd/"))
		hasPassed = false;

	filename_setPath(fn, "/usr/local/asd/");
	if (strcmp(fn->fullName, "/usr/local/asd/"))
		hasPassed = false;

	filename_setPath(fn, NULL);
	if (strcmp(fn->fullName, ""))
		hasPassed = false;

	filename_setPath(fn, "asd/asd/");
	if (strcmp(fn->fullName, "asd/asd/"))
		hasPassed = false;

	filename_del(&fn);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
filename_setPrefix_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_new();

	filename_setPrefix(fn, "easy prefix");
	if (strcmp(fn->fullName, "easy prefix"))
		hasPassed = false;

	filename_setPath(fn, "asd/");
	if (strcmp(fn->fullName, "asd/easy prefix"))
		hasPassed = false;

	filename_del(&fn);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
filename_setQualifier_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_new();

	filename_setQualifier(fn, "_hh2h");
	if (strcmp(fn->fullName, "_hh2h"))
		hasPassed = false;

	filename_setPrefix(fn, "#$%");
	filename_setQualifier(fn, "_hh23");
	if (strcmp(fn->fullName, "#$%_hh23"))
		hasPassed = false;

	filename_del(&fn);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
filename_setSuffix_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_new();

	filename_setSuffix(fn, ".dat");
	if (strcmp(fn->fullName, ".dat"))
		hasPassed = false;

	filename_setPath(fn, "asd/");
	filename_setPrefix(fn, "test");
	filename_setSuffix(fn, ".txt@'ho");
	if (strcmp(fn->fullName, "asd/test.txt@'ho"))
		hasPassed = false;

	filename_setPath(fn, "");
	filename_setPrefix(fn, "test");
	filename_setSuffix(fn, ".tyt@'ho");
	if (strcmp(fn->fullName, "test.tyt@'ho"))

	filename_del(&fn);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
filename_getFullName_test(void)
{
	bool       hasPassed = true;
	int        rank      = 0;
	filename_t fn;
#ifdef XMEM_TRACK_MEM
	size_t     allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	fn = filename_new();

	if (strcmp(filename_getFullName(fn), ""))
		hasPassed = false;

	filename_setPath(fn, "asd/");
	filename_setPrefix(fn, "test");
	filename_setSuffix(fn, ".txt");
	if (strcmp(filename_getFullName(fn), "asd/test.txt"))
		hasPassed = false;

	filename_del(&fn);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
