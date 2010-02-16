// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "grid_config.h"
#include "gridRegular.h"
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include <inttypes.h>
#include <assert.h>


/*--- Implemention of main structure ------------------------------------*/
struct gridRegular_struct {
	uint32_t   ndims;
	uint32_t   *dims;
	char       *gridName;
	uint32_t   numSlabs;
	gridSlab_t *slabs;
};


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridRegular_t
gridRegular_newWithoutData(uint32_t       ndims,
                           const uint32_t *dims,
                           const char     *gridName)
{
	gridRegular_t grid = xmalloc(sizeof(struct gridRegular_struct));
	grid->ndims = ndims;
	grid->dims  = xmalloc(sizeof(uint32_t) * ndims);
	for (uint32_t i = 0; i < ndims; i++)
		grid->dims[i] = dims[i];
	grid->gridName = xstrdup(gridName);
	grid->numSlabs = UINT32_C(0);
	grid->slabs    = NULL;
	return grid;
}

#ifdef WITH_SILO
extern void
gridRegular_writeSilo(gridRegular_t grid, const char *siloBaseName)
{
	int    rank          = 0;
	int    size          = 1;
	DBfile *f            = NULL;
	char   *siloFileName = NULL;
	char   *meshName     = NULL;
	char   *varName      = NULL;

	assert(grid != NULL);
	assert(siloBaseName != NULL);

#  ifdef WITH_MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_rank(MPI_COMM_WORLD, &size);
#  endif
	f        = local_openSiloFile(siloBaseName, rank);

	meshName = xmalloc(sizeof(char) * (strlen(siloBaseName) + 40));
	varName  = xmalloc(sizeof(char) * (strlen(siloBaseName) + 40));
	for (uint32_t i = 0; i < grid->numSlabs; i++) {
		sprintf(meshName, "%s.%05i", grid->gridName, rank);
		sprintf(varName, "VAR.%05i", rank);
		gridSlab_writeSilo(grid->slabs[i], f, meshName, varName,
		                   grid->ndims, grid->dims);
	}
	DBClose(f);
	xfree(varName);
	xfree(meshName);
	if (rank == 0) {
		local_writeMasterSilo(siloBaseName, grid, size);
	}
	xfree(siloFileName);
} /* gridRegular_writeSilo */

#endif

extern void
gridRegular_del(gridRegular_t *grid)
{
	assert(grid != NULL);
	assert(*grid != NULL);
	for (uint32_t i = 0; i < (*grid)->numSlabs; i++)
		gridSlab_del(&((*grid)->slabs[i]));
	if ((*grid)->slabs != NULL)
		xfree((*grid)->slabs);
	xfree((*grid)->dims);
	xfree((*grid)->gridName);
	xfree(*grid);
	*grid = NULL;
}

/*--- Implementations of local functions --------------------------------*/
#ifdef WITH_SILO
static DBfile *
local_openSiloFile(const char *siloBaseName, int rank)
{
	DBfile *f;
	char   *siloFileName = NULL;

	siloFileName = xmalloc(sizeof(char) * (strlen(siloBaseName) + 40));
	sprintf(siloFileName, "%s.%05i.silo", siloBaseName, rank);
	f            = DBCreate(siloFileName,
	                        DB_CLOBBER,
	                        DB_LOCAL,
	                        NULL,
	                        DB_PDB);
	xfree(siloFileName);

	return f;
}

static void
local_writeMasterSiloFile(const char    *siloBaseName,
                          gridRegular_t grid,
                          int           size)
{
	char **meshNames;
	int  *meshTypes;
	char **varNames;
	int  *varTypes;
	char *fileName = xmalloc(sizeof(char) * (strlen(siloBaseName) + 40));

	meshNames = xmalloc(sizeof(char *) * size * 2);
	varNames  = meshNames + size;
	meshTypes = xmalloc(sizeof(int *) * size * 2);
	varTypes  = meshTypes + size;
	sprintf(siloFileName, "%s.silo", siloBaseName);
	f         = DBCreate(siloFileName, DB_CLOBBER, DB_LOCAL, NULL, DB_PDB);
	for (int i = 0; i < size; i++) {
		sprintf(fileName, "%s.%05i.silo", siloBaseName, rank);
		meshNames[i] = xmalloc(strlen(fileName) + 40);
		sprintf(meshNames[i], "%s:%s.%05i", fileName, grid->gridName, i);
	}
	DBClose(f);
	xfree(meshTypes);
	xfree(meshNames);
}

#endif
