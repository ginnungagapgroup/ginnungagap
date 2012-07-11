// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file  libg9p/g9pMaskIO_tests.c
 * @ingroup  libg9pMaskIOTest
 * @brief  Implements the tests for @ref libg9pMaskIO.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pMaskIO_tests.h"
#include "g9pMaskIO.h"
#include "g9pHierarchyIO.h"
#include <stdio.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pMaskIO_verifyCreationFromIniExternalHierarchy(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	parse_ini_t ini = parse_ini_open("tests/masks.ini");
	if (ini == NULL) {
		fprintf(stderr, "Could not open tests/masks.ini\n");
		exit(EXIT_FAILURE);
	}

	g9pHierarchy_t h = g9pHierarchyIO_newFromIni(ini, "Hierarchy");
	g9pMask_t mask = g9pMaskIO_newFromIni(ini, "Mask1", h);

	g9pMask_del(&mask);

	parse_ini_close(&ini);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

extern bool
g9pMaskIO_verifyCreationFromIniWithHierarchyFromIni(void)
{
	bool        hasPassed = true;
	int         rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t      allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	parse_ini_t ini = parse_ini_open("tests/masks.ini");
	if (ini == NULL) {
		fprintf(stderr, "Could not open tests/masks.ini\n");
		exit(EXIT_FAILURE);
	}

	g9pMask_t mask = g9pMaskIO_newFromIni(ini, "Mask1", NULL);

	g9pMask_del(&mask);

	parse_ini_close(&ini);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
