// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "gridPoint.h"
#include <assert.h>
#include <stdint.h>
#include "../libutil/xmem.h"
#include "../libutil/varArr.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridPatch_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridPatch_t
gridPatch_new(gridPointUint32_t idxLo, gridPointUint32_t idxHi)
{
	gridPatch_t gridPatch;

	for (int i = 0; i < NDIM; i++) {
		assert(idxLo[i] <= idxHi[i]);
	}

	gridPatch = xmalloc(sizeof(struct gridPatch_struct));
	gridPatch->numCells = UINT64_C(1);
	for (int i = 0; i < NDIM; i++) {
		gridPatch->idxLo[i] = idxLo[i];
		gridPatch->dims[i]  = idxHi[i] - idxLo[i] + 1;
		gridPatch->numCells *= gridPatch->dims[i];
	}
	gridPatch->varData = varArr_new(0);

	return gridPatch;
}

extern void
gridPatch_del(gridPatch_t *gridPatch)
{
	int numVarData;

	assert(gridPatch != NULL && *gridPatch != NULL);

	numVarData = varArr_getLength((*gridPatch)->varData);
	for (int i=0; i<numVarData; i++) {
		xfree(varArr_remove((*gridPatch)->varData, 0));
	}
	varArr_del(&((*gridPatch)->varData));
	xfree(*gridPatch);

	*gridPatch = NULL;
}

extern uint32_t
gridPatch_getOneDim(gridPatch_t patch, int idxOfDim)
{
	assert(patch != NULL);
	assert(idxOfDim >= 0 && idxOfDim < NDIM);

	return patch->dims[idxOfDim];
}

extern void
gridPatch_getIdxLo(gridPatch_t patch, gridPointUint32_t idxLo)
{
	assert(patch != NULL);

	for (int i=0; i<NDIM; i++)
		idxLo[i] = patch->idxLo[i];
}

extern int
gridPatch_attachVarData(gridPatch_t patch, gridVar_t var)
{
	void *data;

	assert(patch != NULL);
	assert(var != NULL);

	data = xmalloc(gridVar_getSizePerElement(var)*patch->numCells);

	return varArr_insert(patch->varData, data);
}

extern void *
gridPatch_detachVarData(gridPatch_t patch, int idxOfVarData)
{
	assert(idxOfVarData >= 0
	       && idxOfVarData < varArr_getLength(patch->varData));

	return varArr_remove(patch->varData, idxOfVarData);
}

/*--- Implementations of local functions --------------------------------*/
