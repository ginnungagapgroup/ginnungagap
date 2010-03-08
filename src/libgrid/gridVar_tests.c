// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVar_tests.h"
#include "gridVar.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Implemention of main structure ------------------------------------*/
#include "gridVar_adt.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include <stdio.h>


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridVar_new_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	if (rank == 0)
		printf("Testing %s... ", __func__);

	return hasPassed ? true : false;
}

extern bool
gridVar_del_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	if (rank == 0)
		printf("Testing %s... ", __func__);

	return hasPassed ? true : false;
}

extern bool
gridVar_realloc_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	if (rank == 0)
		printf("Testing %s... ", __func__);

	return hasPassed ? true : false;
}

extern bool
gridVar_free_test(void)
{
	bool hasPassed = true;
	int  rank      = 0;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif
	if (rank == 0)
		printf("Testing %s... ", __func__);

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
