// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "grid_config.h"
#include "gridSlab.h"
#include "../libutil/xmem.h"
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include <inttypes.h>
#include <assert.h>


/*--- Implemention of main structure ------------------------------------*/
struct gridSlab_struct {
	uint32_t coordStart;
	uint32_t thicknessInCells;
	uint64_t numTotalCells;
	fpv_t    *data;
	bool     freeDataOnDel;
};


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridSlab_t
gridSlab_newWithoutData(uint32_t coordStart, uint32_t thicknessInCells)
{
	gridSlab_t slab = xmalloc(sizeof(struct gridSlab_struct));
	slab->coordStart       = coordStart;
	slab->thicknessInCells = thicknessInCells;
	slab->numTotalCells    = UINT64_C(0);
	slab->data             = NULL;
	slab->freeDataOnDel    = false;
	return slab;
}

extern void
gridSlab_allocateData(gridSlab_t slab, uint64_t numTotalCells)
{
	assert(slab != NULL);
	assert(slab->data == NULL);
	assert(numTotalCells > slab->thicknessInCells);

	slab->data          = xmalloc(sizeof(fpv_t) * numTotalCells);
	slab->numTotalCells = numTotalCells;
	slab->freeDataOnDel = true;
}

extern void
gridSlab_addDataReference(gridSlab_t slab,
                          fpv_t      *data,
                          uint64_t   numTotalCells)
{
	assert(slab != NULL);
	assert(slab->data == NULL);
	assert(data != NULL);
	assert(numTotalCells > slab->thicknessInCells);

	slab->data          = data;
	slab->numTotalCells = numTotalCells;
	slab->freeDataOnDel = false;
}

extern void
gridSlab_setFreeDataOnDel(gridSlab_t slab, bool choice)
{
	assert(slab != NULL);
	slab->freeDataOnDel = choice;
}

#ifdef WITH_SILO
#  ifdef ENABLE_DOUBLE
#    define DATATYPE DB_DOUBLE
#  else
#    define DATATYPE DB_FLOAT
#  endif
extern void
gridSlab_writeSilo(gridSlab_t slab,
                   DBfile     *f,
                   const char *meshname,
                   const char *varname,
                   int        ndims,
                   const int  *dims)
{
	fpv_t **coords  = xmalloc(sizeof(fpv_t *) * ndims);
	int   *dimsSlab = xmalloc(sizof(int) * ndims * 2);
	int   *start    = dimsSlab + ndims;
	int   i, j;

	dimsSlab[0] = slab->thicknessInCells + 1;
	start[0]    = coordStart;
	for (i = 1; i < ndims; i++) {
		dimsSlab[i] = dims[i] + 1;
		start[i]    = 0;
	}
	for (i = 0; i < ndims; i++) {
		coords[i] = xmalloc(sizeof(fpv_t) * dimsSlab[i]);
		for (j = 0; j < dimsSlab[i]; j++) {
			coords[i][j] = start[i] + j;
		}
	}
	DBPutQuadmesh(f, meshname, NULL, coords, dims, NDIMS,
	              SILO_DATATYPE, DB_COLLINEAR, NULL);
	for (i = 0; i < ndims; i++) {
		dims[i] -= 1;
	}
	DBPutQuadvar1(f, varname, meshname, slab->data, dims, ndims, NULL,
	              0, DATAYPE, DB_ZONECENT, NULL);
	for (i = ndims - 1; i >= 0; i--) {
		xfree(coords[i]);
	}
	xfree(dimsSlab);
	xfree(coords);
}

#endif

extern void
gridSlab_del(gridSlab_t *slab)
{
	assert(slab != NULL);
	assert(*slab != NULL);

	if ((*slab)->freeDataOnDel) {
		if ((*slab)->data != NULL)
			xfree((*slab)->data);
	}
	xfree(*slab);
	*slab = NULL;
}

/*--- Implementations of local functions --------------------------------*/
