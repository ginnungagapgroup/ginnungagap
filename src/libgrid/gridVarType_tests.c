// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarType_tests.h"
#include "gridVarType.h"
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


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridVarType_sizeof_test(void)
{
	bool          hasPassed = true;
	int           rank      = 0;
#ifdef XMEM_TRACK_MEM
	size_t        allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (gridVarType_sizeof(GRIDVARTYPE_INT) != sizeof(int))
		hasPassed = false;
	if (gridVarType_sizeof(GRIDVARTYPE_DOUBLE) != sizeof(double))
		hasPassed = false;
	if (gridVarType_sizeof(GRIDVARTYPE_FPV) != sizeof(fpv_t))
		hasPassed = false;
#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
