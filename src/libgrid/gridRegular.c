// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/varArr.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include <inttypes.h>
#include <assert.h>
#ifdef WITH_SILO
#  include <silo.h>
#  include "../libutil/siloTools.h"
#endif
#include <stdio.h>


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegular_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static gridRegular_t
local_alloc(void);

inline static void
local_resetDelta(gridRegular_t grid);

inline static void
local_resetGridLocalOrigin(gridRegular_t grid);

inline static void
local_resetGridLocalExtent(gridRegular_t grid);


#ifdef WITH_MPI
static void
local_mpiSetCommGridInfo(gridRegular_t grid, MPI_Comm mpiCommGrid);

static void
local_mpiSetCartDims(gridRegular_t grid, gridPointInt_t nProcs);

static void
local_mpiSetCommCartInfo(gridRegular_t grid);

static void
local_mpiCalcLocalVals(gridRegular_t grid);

#endif

#ifdef WITH_SILO
static void
local_siloAddMesh(gridRegular_t grid, DBfile *file, int rank, int size);

static void
local_siloAddMasterMesh(gridRegular_t grid,
                        DBfile        *file,
                        int           size,
                        const char    *siloBaseName);

#endif


/*--- Implementations of exported functios ------------------------------*/
extern gridRegular_t
gridRegular_newWithoutData(const char      *gridName,
                           gridPointSize_t dims)
{
	gridRegular_t grid;

	assert(gridName != NULL);

	grid                 = local_alloc();
	grid->gridName       = xstrdup(gridName);
	grid->globalNumCells = UINT64_C(1);
	grid->localNumCells  = UINT64_C(1);
	for (int i = 0; i < NDIM; i++) {
		grid->globalDims[i]   = dims[i];
		grid->globalNumCells *= grid->globalDims[i];
		grid->localDims[i]    = dims[i];
		grid->localNumCells  *= grid->localDims[i];
	}
	local_resetDelta(grid);
	local_resetGridLocalOrigin(grid);
	local_resetGridLocalExtent(grid);

	return grid;
}

extern void
gridRegular_del(gridRegular_t *grid)
{
	assert(grid != NULL && *grid != NULL);

	xfree((*grid)->gridName);
	xfree(*grid);
	*grid = NULL;
}

extern void
gridRegular_setOrigin(gridRegular_t grid, gridPointDbl_t origin)
{
	assert(grid != NULL);

	for (int i = 0; i < NDIM; i++) {
		grid->globalOrigin[i] = origin[i];
	}
	local_resetGridLocalOrigin(grid);
}

extern void
gridRegular_setExtent(gridRegular_t grid, gridPointDbl_t extent)
{
	assert(grid != NULL);

	for (int i = 0; i < NDIM; i++) {
		grid->globalExtent[i] = extent[i];
	}
	local_resetDelta(grid);
	local_resetGridLocalOrigin(grid);
	local_resetGridLocalExtent(grid);
}

extern int
gridRegular_attachVar(gridRegular_t grid, gridVar_t var)
{
	assert(grid != NULL);
	assert(var != NULL);

	return varArr_insert(grid->vars, var);
}

extern gridVar_t
gridRegular_detachVar(gridRegular_t grid, int varToDetach)
{
	assert(grid != NULL)
	assert(varToDetach >= 0
	       && varToDetach < varArr_getLength(grid->Vars));

	return varArr_remove(grid->vars, varToDetach);
}

extern void
gridRegular_reallocVar(gridRegular_t grid, int varToRealloc)
{
	assert(grid != NULL);
	assert(varToRealloc >= 0
	       && varToAlloc < varArr_getLength(grid->Vars));
	assert(grid->localNumCells > UINT64_C(0));

	gridVar_realloc(varArr_getElementHandle(grid->Vars, varToRealloc),
	                (size_t)grid->localNumCells);
}

extern void
gridRegular_reallocAllVars(gridRegular_t grid)
{
	assert(grid != NULL);
	assert(grid->localNumCells > UINT64_C(0));

	for (int i=0; i<arArr_getLength(grid->Vars); i++)
		gridRegular_reallocVar(grid, i);
}

extern void
gridRegular_deallocVar(gridRegular_t grid, int varToDealloc)
{
	assert(grid != NULL);
	assert(varToDealloc >= 0
	       && varToDealloc < varArr_getLength(grid->Vars));

	gridVar_dealloc(varArr_getElementHandle(grid->Vars, varToDealloc));
}

extern void
gridRegular_deallocAllVars(gridRegular_t grid)
{
	assert(grid != NULL);

	for (int i=0; i<arArr_getLength(grid->Vars); i++)
		gridRegular_deallocVar(grid, i);
}

#ifdef WITH_MPI
extern void
gridRegular_mpiSetDistribution(gridRegular_t  grid,
                               gridPointInt_t nProcs,
                               MPI_Comm       mpiCommGrid)
{
	gridPointInt_t periodic;

	assert(grid != NULL);
	assert(mpiCommGrid != MPI_COMM_NULL);

	local_mpiSetCommGridInfo(grid, mpiCommGrid);
	local_mpiSetCartDims(grid, nProcs);

	for (int i = 0; i < NDIM; i++)
		periodic[i] = 1;
	(void)MPI_Cart_create(grid->mpiCommGrid, NDIM, grid->mpiNumProcs,
	                      periodic, 1, &(grid->mpiCommCart));

	local_mpiSetCommCartInfo(grid);
	local_mpiCalcLocalVals(grid);
}

#endif

#ifdef WITH_SILO
extern void
gridRegular_writeSilo(gridRegular_t grid, const char *siloBaseName)
{
	char   *fname;
	int    size  = 1;
	int    rank  = 0;
	DBfile *file = NULL;
#  ifdef WITH_MPI
	size = grid->mpiCommGridSize;
	rank = grid->mpiCommGridRank;
#  endif

	assert(grid != NULL);
	assert(silBaseName != NULL);

	fname = siloTools_getFileName(siloBaseName, rank, size);

	file  = DBCreate(fname, DB_CLOBBER, DB_LOCAL, NULL, DB_PDB);
	local_siloAddMesh(grid, file, rank, size);
	DBClose(file);
	xfree(fname);

	if (rank == 0) {
		fname = siloTools_getMasterFileName(siloBaseName);
		file  = DBCreate(fname, DB_CLOBBER, DB_LOCAL, NULL, DB_PDB);
		local_siloAddMasterMesh(grid, file, size, siloBaseName);
		DBClose(file);
		xfree(fname);
	}
}

#endif


/*--- Implementations of local functions --------------------------------*/
static gridRegular_t
local_alloc(void)
{
	gridRegular_t grid;

	grid                  = xmalloc(sizeof(struct gridRegular_struct));
	grid->gridName        = NULL;
	grid->globalNumCells  = UINT64_C(0);
	grid->localNumCells   = UINT64_C(0);
#ifdef WITH_MPI
	grid->mpiCommGrid     = MPI_COMM_NULL;
	grid->mpiCommGridSize = 1;
	grid->mpiCommGridRank = 0;
	grid->mpiCommCart     = MPI_COMM_NULL;
	grid->mpiCommCartSize = 1;
	grid->mpiCommCartRank = 0;
#endif
	for (int i = 0; i < NDIM; i++) {
		grid->globalOrigin[i]      = 0.0;
		grid->globalExtent[i]      = 1.0;
		grid->globalDims[i]        = 0;
		grid->delta[i]             = 0.0;
		grid->localOrigin[i]       = grid->globalOrigin[i];
		grid->localExtent[i]       = grid->globalExtent[i];
		grid->localDims[i]         = grid->globalDims[i];
		grid->localStart[i]        = 0;
#ifdef WITH_MPI
		grid->mpiCommCartCoords[i] = 0;
		grid->mpiNumProcs[i]       = 1;
#endif
	}

	return grid;
}

inline static void
local_resetDelta(gridRegular_t grid)
{
	for (int i = 0; i < NDIM; i++) {
		grid->delta[i]  = grid->globalExtent[i] - grid->globalOrigin[i];
		grid->delta[i] /= (grid->globalDims[i]);
	}
}

inline static void
local_resetGridLocalOrigin(gridRegular_t grid)
{
	for (int i = 0; i < NDIM; i++) {
		grid->localOrigin[i] = grid->globalOrigin[i]
		                       + grid->delta[i] * grid->localStart[i];
	}
}

inline static void
local_resetGridLocalExtent(gridRegular_t grid)
{
	for (int i = 0; i < NDIM; i++) {
		grid->localExtent[i] = grid->delta[i] * grid->localDims[i];
	}
}

#ifdef WITH_MPI
static void
local_mpiSetCommGridInfo(gridRegular_t grid, MPI_Comm mpiCommGrid)
{
	grid->mpiCommGrid = mpiCommGrid;
	MPI_Comm_size(grid->mpiCommGrid, &(grid->mpiCommGridSize));
	MPI_Comm_rank(grid->mpiCommGrid, &(grid->mpiCommGridRank));
}

static void
local_mpiSetCartDims(gridRegular_t grid, gridPointInt_t nProcs)
{
	for (int i = 0; i < NDIM; i++)
		grid->mpiNumProcs[i] = nProcs[i];
	MPI_Dims_create(grid->mpiCommGridSize, NDIM, grid->mpiNumProcs);
}

static void
local_mpiSetCommCartInfo(gridRegular_t grid)
{
	MPI_Comm_size(grid->mpiCommCart, &(grid->mpiCommCartSize));
	MPI_Comm_rank(grid->mpiCommCart, &(grid->mpiCommCartRank));
	MPI_Cart_coords(grid->mpiCommCart, grid->mpiCommCartRank, NDIM,
	                grid->mpiCommCartCoords);
}

static void
local_mpiCalcLocalVals(gridRegular_t grid)
{
	int idealDistri;
	int hangoverDistri;
	int correction;

	// For each dim assign cells allowing for a 1 cell imbalance
	for (int i = 0; i < NDIM; i++) {
		idealDistri         = grid->globalDims[i] / grid->mpiNumProcs[i];
		hangoverDistri      = grid->globalDims[i] % grid->mpiNumProcs[i];
		grid->localDims[i]  = (gridSize_t)idealDistri;
		grid->localStart[i] = (gridSize_t)(idealDistri
		                                   * grid->mpiCommCartCoords[i]);
		if (grid->mpiCommCartCoords[i] < hangoverDistri)
			grid->localDims[i]++;
		correction = (grid->mpiCommCartCoords[i] < hangoverDistri)
		             ? grid->mpiCommCartCoords[i]
					 : hangoverDistri;
		grid->localStart[i] = (gridSize_t)(grid->localStart[i] + correction);
	}
	local_resetGridLocalOrigin(grid);
	local_resetGridLocalExtent(grid);
}

#endif

#ifdef WITH_SILO
static void
local_siloAddMesh(gridRegular_t grid, DBfile *file, int rank, int size)
{
	double *coords[NDIM];
	char   *gridName;
	int    dims[NDIM];

	gridName = siloTools_getGridName(grid->gridName, rank, size);
	for (int i = 0; i < NDIM; i++) {
		// need left and right coords of the cell, hence the +1
		coords[i] = xmalloc(sizeof(double) * (grid->localDims[i] + 1));
		for (int j = 0; j < grid->localDims[i] + 1; j++)
			coords[i][j] = grid->localOrigin[i] + j * grid->delta[i];
		dims[i] = grid->localDims[i] + 1;
	}

	DBPutQuadmesh(file, gridName, NULL, coords, dims, NDIM,
	              DB_DOUBLE, DB_COLLINEAR, NULL);

	for (int i = 0; i < NDIM; i++)
		xfree(coords[i]);
	xfree(gridName);
}

static void
local_siloAddMasterMesh(gridRegular_t grid,
                        DBfile        *file,
                        int           size,
                        const char    *siloBaseName)
{
	int  *gridTypes;
	char **gridNames;
	char *siloFileName;
	int  i;

	gridTypes = xmalloc(sizeof(int) * size);
	gridNames = xmalloc(sizeof(char *) * size);

	for (i = 0; i < size; i++) {
		siloFileName = siloTools_getFileName(siloBaseName, i, size);
		gridTypes[i] = DB_QUAD_RECT;
		gridNames[i] = siloTools_getGridName(grid->gridName, i, size);
		gridNames[i] = siloTools_prependFileName(gridNames[i], siloFileName);
		xfree(siloFileName);
	}

	DBPutMultimesh(file, grid->gridName, size, gridNames, gridTypes, NULL);

	for (i = 0; i < size; i++)
		xfree(gridNames[i]);
	xfree(gridNames);
	xfree(gridTypes);
}

#endif
