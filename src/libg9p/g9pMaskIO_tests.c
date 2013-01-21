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
#ifdef WITH_HDF5
#  include "g9pMaskCreator.h"
#  include "../libgrid/gridReaderFactory.h"
#  include "../libgrid/gridWriterFactory.h"
#endif
#ifdef WITH_SILO
#  include "../libgrid/gridWriterSilo.h"
#  include <silo.h>
#endif


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
#ifdef WITH_HDF5
static bool
local_checkMasksAreEqual(g9pMask_t m1, g9pMask_t m2);


#endif


/*--- Implementations of exported functions -----------------------------*/
extern bool
g9pMaskIO_verifyCreationFromIniExternalHierarchy(void)
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

	parse_ini_t ini = parse_ini_open("tests/masks.ini");
	if (ini == NULL) {
		fprintf(stderr, "Could not open tests/masks.ini\n");
		exit(EXIT_FAILURE);
	}

	g9pHierarchy_t h    = g9pHierarchyIO_newFromIni(ini, "Hierarchy");
	g9pMask_t      mask = g9pMaskIO_newFromIni(ini, "Mask1", h);

	g9pMask_del(&mask);

	parse_ini_close(&ini);

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskIO_verifyCreationFromIniExternalHierarchy */

extern bool
g9pMaskIO_verifyCreationFromIniWithHierarchyFromIni(void)
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

extern bool
g9pMaskIO_verifyReadWriteSimple(void)
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

#ifdef WITH_HDF5
	parse_ini_t ini = parse_ini_open("tests/maskIO.ini");
	g9pMask_t   m   = g9pMaskIO_newFromIni(ini,
	                                       "Mask",
	                                       NULL);
	g9pMask_t m2    = g9pMaskIO_newFromIni(ini,
	                                       "Mask",
	                                       NULL);
	const gridPointUint32_t cell[3]
	    = {{16, 16, 16}, {16, 16, 17}, {16, 15, 16}};
	g9pMaskCreator_fromCells(m, 3, cell);

	gridWriter_t w = gridWriterFactory_newWriterFromIni(ini, "Writer");
	g9pMaskIO_write(m, w);
#  ifdef WITH_SILO
	{
		gridWriter_t wVis = (gridWriter_t)gridWriterSilo_new();
		gridWriter_setFileName(wVis,
		                       filename_newFull(NULL, "simpleMask",
		                                        NULL, ".silo"));
		gridWriter_setOverwriteFileIfExists(wVis, true);
		gridWriterSilo_setDbType((gridWriterSilo_t)wVis, DB_HDF5);
		g9pMaskIO_write(m, wVis);
		gridWriter_del(&wVis);
	}
#  endif
	gridWriter_del(&w);

	gridReader_t r = gridReaderFactory_newReaderFromIni(ini, "Reader");
	g9pMaskIO_read(m2, r);
	gridReader_del(&r);

	hasPassed = local_checkMasksAreEqual(m, m2);

	g9pMask_del(&m);
	g9pMask_del(&m2);
	parse_ini_close(&ini);
#else
	if (rank == 0)
		printf("(skipped, as HDF5 is required) ");
#endif

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
} /* g9pMaskIO_verifyReadWriteSimple */

/*--- Implementations of local functions --------------------------------*/
#ifdef WITH_HDF5
static bool
local_checkMasksAreEqual(g9pMask_t m1, g9pMask_t m2)
{
	if (g9pMask_getMaskLevel(m1) != g9pMask_getMaskLevel(m2))
		return false;

	if (g9pMask_getMinLevel(m1) != g9pMask_getMinLevel(m2))
		return false;

	if (g9pMask_getMaxLevel(m1) != g9pMask_getMaxLevel(m2))
		return false;

	if (g9pMask_getTileLevel(m1) != g9pMask_getTileLevel(m2))
		return false;

	if (g9pMask_getTotalNumTiles(m1) != g9pMask_getTotalNumTiles(m2))
		return false;

	{
		const uint32_t *tmp1 = g9pMask_getNumTiles(m1);
		const uint32_t *tmp2 = g9pMask_getNumTiles(m2);
		for (int i = 0; i < NDIM; i++) {
			if (tmp1[i] != tmp2[i])
				return false;
		}
	}

	if (g9pMask_getNumCellsInMaskTile(m1) !=
	    g9pMask_getNumCellsInMaskTile(m2))
		return false;

	for (int i = 0; i < g9pMask_getTotalNumTiles(m1); i++) {
		const int8_t *d1 = g9pMask_getTileData(m1, i);
		const int8_t *d2 = g9pMask_getTileData(m2, i);

		for (uint64_t i = 0; i < g9pMask_getNumCellsInMaskTile(m1); i++) {
			if (d1[i] != d2[i])
				return false;
		}
	}

	return true;
} /* local_checkMasksAreEqual */

#endif
