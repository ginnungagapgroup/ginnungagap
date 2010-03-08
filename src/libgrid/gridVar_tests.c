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
#define TESTNAME "asdk1l2kl3masdkwkejqwe"


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern bool
gridVar_new_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	gridVar_t var;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new(TESTNAME, GRIDVARTYPES_FPVVEC);
	if (strcmp(var->name, TESTNAME) != 0)
		hasPassed = false;
	if (var->type != GRIDVARTYPES_FPVVEC)
		hasPassed = false;
	if (var->numElements != 0)
		hasPassed = false;
	if (var->sizePerElement
	    != gridVarTypes_getSizePerElement(GRIDVARTYPES_FPVVEC))
		hasPassed = false;
	if (var->data != NULL)
		hasPassed = false;
	gridVar_del(&var);

	return hasPassed ? true : false;
}

extern bool
gridVar_del_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	gridVar_t var;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new(TESTNAME, GRIDVARTYPES_FPVVEC);
	gridVar_del(&var);
	if (var != NULL)
		hasPassed = false;

	var = gridVar_new(TESTNAME, GRIDVARTYPES_FPVVEC);
	gridVar_realloc(var, 2934);
	gridVar_del(&var);
	if (var != NULL)
		hasPassed = false;

	return hasPassed ? true : false;
}

extern bool
gridVar_realloc_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	gridVar_t var;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new(TESTNAME, GRIDVARTYPES_FPVVEC);
	gridVar_realloc(var, 2934);
	if (var->numElements != 2934)
		hasPassed = false;
	if (var->data == NULL)
		hasPassed = false;
	gridVar_del(&var);

	return hasPassed ? true : false;
}

extern bool
gridVar_free_test(void)
{
	bool      hasPassed = true;
	int       rank      = 0;
	gridVar_t var;
#ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
#endif

	if (rank == 0)
		printf("Testing %s... ", __func__);

	var = gridVar_new(TESTNAME, GRIDVARTYPES_FPVVEC);
	gridVar_realloc(var, 2934);
	gridVar_free(var);
	if (var->numElements != 0)
		hasPassed = false;
	if (var->data != NULL)
		hasPassed = false;
	gridVar_del(&var);

	return hasPassed ? true : false;
}

/*--- Implementations of local functions --------------------------------*/
