// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libg9p/g9pFieldID_tests.c
 * @ingroup  libg9pFieldID
 * @brief  Implements the test for the field ID.
 */


/*--- Includes ----------------------------------------------------------*/
#include "g9pConfig.h"
#include "g9pFieldID_tests.h"
#include "g9pFieldID.h"
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
g9pFieldID_getStr_test(void)
{
	bool         hasPassed = true;
	int          rank      = 0;
	g9pFieldID_t g9pFieldID;
#ifdef XMEM_TRACK_MEM
	size_t       allocatedBytes = global_allocated_bytes;
#endif
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	if (strcmp("wn", g9pFieldID_getStr(G9PFIELDID_WN)) != 0)
		hasPassed = false;
	if (strcmp("delta", g9pFieldID_getStr(G9PFIELDID_DELTA)) != 0)
		hasPassed = false;
	if (strcmp("velx", g9pFieldID_getStr(G9PFIELDID_VX)) != 0)
		hasPassed = false;
	if (strcmp("vely", g9pFieldID_getStr(G9PFIELDID_VY)) != 0)
		hasPassed = false;
	if (strcmp("velz", g9pFieldID_getStr(G9PFIELDID_VZ)) != 0)
		hasPassed = false;

#ifdef XMEM_TRACK_MEM
	if (allocatedBytes != global_allocated_bytes)
		hasPassed = false;
#endif

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
