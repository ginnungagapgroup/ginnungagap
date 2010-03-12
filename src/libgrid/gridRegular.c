// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "gridVar.h"
#include "gridPatch.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/xstring.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegular_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
inline static void
local_resetDelta(gridRegular_t gridRegular);

inline static void
local_freePatches(gridRegular_t grid);

inline static void
local_freeVars(gridRegular_t grid);

inline static void
local_addVarToAllPatches(gridRegular_t grid, gridVar_t var, int idxOfVar);

inline static void
local_removeVarFromAllPatches(gridRegular_t grid, int idxOfVar);

inline static void
local_addAllVarsToPatch(gridRegular_t grid, gridPatch_t patch);


/*--- Implementations of exported functios ------------------------------*/
extern gridRegular_t
gridRegular_new(const char        *name,
                gridPointDbl_t    origin,
                gridPointDbl_t    extent,
                gridPointUint32_t dims)
{
	gridRegular_t gridRegular;

	for (int i = 0; i < NDIM; i++) {
		assert(dims[i] > 0);
		assert(extent[i] > 0.0);
	}

	gridRegular       = xmalloc(sizeof(struct gridRegular_struct));

	gridRegular->name = xstrdup(name);
	for (int i = 0; i < NDIM; i++) {
		gridRegular->origin[i] = origin[i];
		gridRegular->extent[i] = extent[i];
		gridRegular->dims[i]   = dims[i];
	}
	local_resetDelta(gridRegular);
	gridRegular->patches = varArr_new(0);
	gridRegular->vars    = varArr_new(0);

	return gridRegular;
}

extern void
gridRegular_del(gridRegular_t *gridRegular)
{
	assert(gridRegular != NULL && *gridRegular != NULL);

	xfree((*gridRegular)->name);
	local_freePatches(*gridRegular);
	local_freeVars(*gridRegular);
	xfree(*gridRegular);

	*gridRegular = NULL;
}

extern char *
gridRegular_getName(gridRegular_t grid)
{
	assert(grid != NULL);
	assert(grid->name != NULL);

	return grid->name;
}

extern void
gridRegular_getOrigin(gridRegular_t grid, gridPointDbl_t origin)
{
	assert(grid != NULL);
	assert(origin != NULL);

	for (int i=0; i<NDIM; i++) {
		origin[i] = grid->origin[i];
	}
}

extern void
gridRegular_getDelta(gridRegular_t grid, gridPointDbl_t delta)
{
	assert(grid != NULL);
	assert(delta != NULL);

	for (int i=0; i<NDIM; i++) {
		delta[i] = grid->delta[i];
	}
}

extern int
gridRegular_attachVar(gridRegular_t grid, gridVar_t var)
{
	int idxOfVar;

	assert(grid != NULL);
	assert(var != NULL);

	idxOfVar = varArr_insert(grid->vars, var);
	local_addVarToAllPatches(grid, var, idxOfVar);

	return idxOfVar;
}

extern gridVar_t
gridRegular_detachVar(gridRegular_t grid, int idxOfVar)
{
	assert(grid != NULL);
	assert(idxOfVar >= 0 && idxOfVar < varArr_getLength(grid->vars));

	local_removeVarFromAllPatches(grid, idxOfVar);

	return varArr_remove(grid->vars, idxOfVar);
}

extern int
gridRegular_getNumPatches(gridRegular_t grid)
{
	assert(grid != NULL);

	return varArr_getLength(grid->patches);
}

extern int
gridRegular_attachPatch(gridRegular_t grid, gridPatch_t patch)
{
	int idxOfPatch;

	assert(grid != NULL);
	assert(patch != NULL);

	local_addAllVarsToPatch(grid, patch);
	idxOfPatch = varArr_insert(grid->patches, patch);

	return idxOfPatch;
}

extern gridPatch_t
gridRegular_detachPatch(gridRegular_t grid, int idxOfPatch)
{
	assert(grid != NULL);
	assert(idxOfPatch >= 0 && idxOfPatch < varArr_getLength(grid->patches));

	return varArr_remove(grid->patches, idxOfPatch);
}

extern gridPatch_t
gridRegular_getPatchHandle(gridRegular_t grid, int idxPatchToGet)
{
	assert(grid != NULL);
	assert(idxPatchToGet >= 0
	       && idxPatchToGet < varArr_getLength(grid->patches));

	return varArr_getElementHandle(grid->patches, idxPatchToGet);
}


/*--- Implementations of local functions --------------------------------*/
inline static void
local_resetDelta(gridRegular_t gridRegular)
{
	for (int i = 0; i < NDIM; i++) {
		gridRegular->delta[i] = gridRegular->extent[i]
		                        / ((double)(gridRegular->dims[i]));
	}
}

inline static void
local_freePatches(gridRegular_t grid)
{
	while (varArr_getLength(grid->patches) != 0) {
		gridPatch_t patch;
		patch = varArr_remove(grid->patches, 0);
		gridPatch_del(&patch);
	}
	varArr_del(&(grid->patches));
}

inline static void
local_freeVars(gridRegular_t grid)
{
	while (varArr_getLength(grid->vars) != 0) {
		gridVar_t var;
		var = varArr_remove(grid->vars, 0);
		gridVar_del(&var);
	}
	varArr_del(&(grid->vars));
}

inline static void
local_addVarToAllPatches(gridRegular_t grid, gridVar_t var, int idxOfVar)
{
	int numPatches = varArr_getLength(grid->patches);

	for (int i = 0; i < numPatches; i++) {
		gridPatch_t patch        = varArr_getElementHandle(grid->patches, i);
		int         idxOfVarData = gridPatch_attachVarData(patch, var);
		assert(idxOfVarData == idxOfVar);
	}
}

inline static void
local_removeVarFromAllPatches(gridRegular_t grid, int idxOfVar)
{
	int numPatches = varArr_getLength(grid->patches);

	for (int i = 0; i < numPatches; i++) {
		gridPatch_t patch = varArr_getElementHandle(grid->patches, i);
		xfree(gridPatch_detachVarData(patch, idxOfVar));
	}
}

inline static void
local_addAllVarsToPatch(gridRegular_t grid, gridPatch_t patch)
{
	int numVars = varArr_getLength(grid->vars);

	for (int i=0; i<numVars; i++) {
		gridVar_t var = varArr_getElementHandle(grid->vars, i);
		gridPatch_attachVarData(patch, var);
	}
}
