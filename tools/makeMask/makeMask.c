// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "makeMaskConfig.h"
#include "makeMask.h"
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "../../src/libgrid/gridRegular.h"
#include "../../src/libgrid/gridRegularDistrib.h"
#include "../../src/libutil/xmem.h"
#include "../../src/libutil/xfile.h"


/*--- Implemention of main structure ------------------------------------*/
#include "makeMask_adt.h"


/*--- Prototypes of local functions -------------------------------------*/
inline static gridRegular_t
local_getGrid(makeMask_t mama);

inline static gridRegularDistrib_t
local_getDistrib(makeMask_t mama);

inline static void
local_createEmptyMask(makeMask_t mama);

inline static void
local_markHighResRegions(makeMask_t mama);

inline static void
local_markLowResRegions(makeMask_t mama);

inline static void
local_writeMask(makeMask_t mama);


/*--- Implementations of exported functios ------------------------------*/
extern makeMask_t
makeMask_newFromIni(parse_ini_t ini, const char *maskSectionName)
{
	makeMask_t mama;

	assert(ini != NULL);
	assert(maskSectionName != NULL);

	mama          = xmalloc(sizeof(struct makeMask_struct));

	mama->setup   = makeMaskSetup_newFromIni(ini, maskSectionName);
	mama->grid    = local_getGrid(mama);
	mama->distrib = local_getDistrib(mama);

	return mama;
}

extern void
makeMask_run(makeMask_t mama)
{
	assert(mama != NULL);

	local_createEmptyMask(mama);
	local_markHighResRegions(mama);
	local_markLowResRegions(mama);
	local_writeMask(mama);
}

extern void
makeMask_del(makeMask_t *mama)
{
	assert(mama != NULL);
	assert(*mama != NULL);

	makeMaskSetup_del(&((*mama)->setup));
	gridRegularDistrib_del(&((*mama)->distrib));
	gridRegular_del(&((*mama)->grid));
	xfree(*mama);

	*mama = NULL;
}

/*--- Implementations of local functions --------------------------------*/
inline static gridRegular_t
local_getGrid(makeMask_t mama)
{
	gridPointDbl_t    origin, extent;
	gridPointUint32_t dims;
	uint32_t          baseGridSize1D;

	baseGridSize1D = makeMaskSetup_getBaseGridSize1D(mama->setup);

	for (int i = 0; i < NDIM; i++) {
		origin[i] = 0.0;
		extent[i] = (double)baseGridSize1D;
		dims[i]   = baseGridSize1D;
	}

	return gridRegular_new("MaskGrid", origin, extent, dims);
}

inline static gridRegularDistrib_t
local_getDistrib(makeMask_t mama)
{
	gridPointInt_t       nProcs;
	gridRegularDistrib_t distrib;

	distrib = gridRegularDistrib_new(mama->grid, nProcs);
#ifdef WITH_MPI
	gridRegularDistrib_initMPI(distrib, nProcs, MPI_COMM_WORLD);
#endif

	return distrib;
}

inline static void
local_createEmptyMask(makeMask_t mama)
{
}

inline static void
local_markHighResRegions(makeMask_t mama)
{
}

inline static void
local_markLowResRegions(makeMask_t mama)
{
}

inline static void
local_writeMask(makeMask_t mama)
{
}
