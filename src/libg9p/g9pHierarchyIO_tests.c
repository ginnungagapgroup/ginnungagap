// Copyright (C) 2012, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pHierarchyIO_tests.c
 * @ingroup  libg9pHierarchyIOTest
 * @brief  Implements the IO tests of @ref libg9pHierarchyIO.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pHierarchyIO_tests.h"
#include "g9pHierarchyIO.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../libutil/parse_ini.h"
#ifdef XMEM_TRACK_MEM
#  include "../libutil/xmem.h"
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pHierarchyIO_verifyConstructionFromIni(void)
{
	bool           hasPassed = true;
	int            rank      = 0;
	g9pHierarchy_t h1, h2, h3;
	parse_ini_t    ini;
#ifdef XMEM_TRACK_MEM
	size_t           allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	ini = parse_ini_open("tests/hierarchies.ini");
	if (ini == NULL) {
		fprintf(stderr, "Could not open tests/hierarchies.ini\n");
		exit(EXIT_FAILURE);
	}

	h1 = g9pHierarchyIO_newFromIni(ini, "Hierarchy1");
	h2 = g9pHierarchyIO_newFromIni(ini, "Hierarchy2");
	h3 = g9pHierarchyIO_newFromIni(ini, "Hierarchy3");

	const uint32_t *h1dims = g9pHierarchy_getDim1Ds(h1, NULL);
	const uint32_t *h2dims = g9pHierarchy_getDim1Ds(h2, NULL);
	const uint32_t *h3dims = g9pHierarchy_getDim1Ds(h3, NULL);

	if (h1dims[3] != h2dims[3] || h1dims[3] != h3dims[3])
		hasPassed = false;

	if (h1dims[1] != h2dims[1] || h1dims[1] != h3dims[1])
		hasPassed = false;

	if (h1dims[0] != h2dims[0] || h1dims[0] != h3dims[0])
		hasPassed = false;

	g9pHierarchy_del(&h3);
	g9pHierarchy_del(&h2);
	g9pHierarchy_del(&h1);

	parse_ini_close(&ini);
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
