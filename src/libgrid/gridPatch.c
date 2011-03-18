// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPatch.h"
#include "gridPoint.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "../libutil/xmem.h"
#include "../libutil/varArr.h"
#include "../libutil/diediedie.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridPatch_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_transposeVar(gridPatch_t patch,
                   int         idxOfVarData,
                   int         dimA,
                   int         dimB);


#if (NDIM == 2)
static void
local_transposeVar_2d(const void              *data,
                      void                    *dataT,
                      const int               size,
                      const gridPointUint32_t dimsT);


#elif (NDIM == 3)
static void
local_transposeVar102_3d(const void              *data,
                         void                    *dataT,
                         const int               size,
                         const gridPointUint32_t dimsT);

static void
local_transposeVar210_3d(const void              *data,
                         void                    *dataT,
                         const int               size,
                         const gridPointUint32_t dimsT);

static void
local_transposeVar021_3d(const void              *data,
                         void                    *dataT,
                         const int               size,
                         const gridPointUint32_t dimsT);

#endif

static inline void
local_getWindowDims(gridPointUint32_t idxLo,
                    gridPointUint32_t idxHi,
                    gridPointUint32_t windowDims,
                    uint64_t          *numCells);


/*--- Implementations of exported functios ------------------------------*/
extern gridPatch_t
gridPatch_new(gridPointUint32_t idxLo, gridPointUint32_t idxHi)
{
	gridPatch_t gridPatch;

	for (int i = 0; i < NDIM; i++) {
		assert(idxLo[i] <= idxHi[i]);
	}

	gridPatch           = xmalloc(sizeof(struct gridPatch_struct));
	gridPatch->numCells = UINT64_C(1);
	for (int i = 0; i < NDIM; i++) {
		gridPatch->idxLo[i]  = idxLo[i];
		gridPatch->dims[i]   = idxHi[i] - idxLo[i] + 1;
		gridPatch->numCells *= gridPatch->dims[i];
	}
	gridPatch->vars    = varArr_new(0);
	gridPatch->varData = varArr_new(0);

	return gridPatch;
}

extern void
gridPatch_del(gridPatch_t *gridPatch)
{
	int       numVarData;
	gridVar_t var;

	assert(gridPatch != NULL && *gridPatch != NULL);

	if ((*gridPatch)->varData != NULL) {
		numVarData = varArr_getLength((*gridPatch)->varData);
		for (int i = 0; i < numVarData; i++) {
			var = gridPatch_detachVar(*gridPatch, 0);
			gridVar_del(&var);
		}
		varArr_del(&((*gridPatch)->varData));
		varArr_del(&((*gridPatch)->vars));
	}
	xfree(*gridPatch);

	*gridPatch = NULL;
}

extern uint32_t
gridPatch_getOneDim(const gridPatch_t patch, int idxOfDim)
{
	assert(patch != NULL);
	assert(idxOfDim >= 0 && idxOfDim < NDIM);

	return patch->dims[idxOfDim];
}

extern uint32_t
gridPatch_getDimActual1D(const gridPatch_t patch, int idxOfVar, int dim)
{
	uint64_t  actualDim;
	gridVar_t var;

	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < varArr_getLength(patch->vars));
	assert(dim >= 0 && dim < NDIM);

	var = gridPatch_getVarHandle(patch, idxOfVar);

	if (gridVar_isFFTWPadded(var) && (dim == 0))
		actualDim = 2 * ((patch->dims[dim]) / 2 + 1);
	else
		actualDim = patch->dims[dim];

	return actualDim;
}

extern void
gridPatch_getDims(const gridPatch_t patch, gridPointUint32_t dims)
{
	assert(patch != NULL);
	assert(dims != NULL);

	for (int i = 0; i < NDIM; i++)
		dims[i] = patch->dims[i];
}

extern void
gridPatch_getDimsActual(const gridPatch_t patch,
                        int               idxOfVar,
                        gridPointUint32_t dimsActual)
{
	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < varArr_getLength(patch->vars));
	assert(dimsActual != NULL);

	for (int i = 0; i < NDIM; i++)
		dimsActual[i] = gridPatch_getDimActual1D(patch, idxOfVar, i);
}

extern uint64_t
gridPatch_getNumCells(const gridPatch_t patch)
{
	assert(patch != NULL);

	return patch->numCells;
}

extern uint64_t
gridPatch_getNumCellsActual(const gridPatch_t patch, int idxOfVar)
{
	uint64_t numCellsActual = 1;

	assert(patch != NULL);
	assert(idxOfVar >= 0 && idxOfVar < varArr_getLength(patch->vars));

	for (int i = 0; i < NDIM; i++)
		numCellsActual *= gridPatch_getDimActual1D(patch, idxOfVar, i);

	return numCellsActual;
}

extern void
gridPatch_getIdxLo(const gridPatch_t patch, gridPointUint32_t idxLo)
{
	assert(patch != NULL);

	for (int i = 0; i < NDIM; i++)
		idxLo[i] = patch->idxLo[i];
}

extern int
gridPatch_attachVar(gridPatch_t patch, gridVar_t var)
{
	gridVar_t varClone;
	int       posVar, posVarData;

	assert(patch != NULL);
	assert(var != NULL);

	varClone   = gridVar_getRef(var);
	posVar     = varArr_insert(patch->vars, varClone);
	posVarData = varArr_insert(patch->varData, NULL);

	if (posVar != posVarData) {
		diediedie(EXIT_FAILURE);
	}

	return posVar;
}

extern gridVar_t
gridPatch_detachVar(gridPatch_t patch, int idxOfVar)
{
	assert(idxOfVar >= 0
	       && idxOfVar < varArr_getLength(patch->vars));

	gridPatch_freeVarData(patch, idxOfVar);
	(void)varArr_remove(patch->varData, idxOfVar);

	return varArr_remove(patch->vars, idxOfVar);
}

extern void *
gridPatch_allocateVarData(gridPatch_t patch, int idxOfVarData)
{
	void *data;

	assert(patch != NULL);
	assert(idxOfVarData >= 0
	       && idxOfVarData < varArr_getLength(patch->varData));

	data = varArr_getElementHandle(patch->varData, idxOfVarData);

	if (data == NULL) {
		gridVar_t var;
		uint64_t  numCellsToAllocate = 1;
		var                = gridPatch_getVarHandle(patch, idxOfVarData);
		numCellsToAllocate = gridPatch_getNumCellsActual(patch,
		                                                 idxOfVarData);
		data               = gridVar_getMemory(var, numCellsToAllocate);
		(void)varArr_replace(patch->varData, idxOfVarData, data);
	}

	return data;
}

extern void
gridPatch_freeVarData(gridPatch_t patch, int idxOfVarData)
{
	gridVar_t var;
	void      *data;

	assert(idxOfVarData >= 0
	       && idxOfVarData < varArr_getLength(patch->varData));

	var  = varArr_getElementHandle(patch->vars, idxOfVarData);
	data = varArr_replace(patch->varData, idxOfVarData, NULL);
	if (data != NULL)
		gridVar_freeMemory(var, data);
}

extern void
gridPatch_replaceVarData(gridPatch_t patch, int idxOfVarData, void *newData)
{
	void      *oldData;
	gridVar_t var;

	assert(patch != NULL);
	assert(idxOfVarData >= 0
	       && idxOfVarData < varArr_getLength(patch->varData));

	oldData = varArr_replace(patch->varData, idxOfVarData, newData);
	var     = gridPatch_getVarHandle(patch, idxOfVarData);
	if (oldData != NULL)
		gridVar_freeMemory(var, oldData);
}

extern gridVar_t
gridPatch_getVarHandle(const gridPatch_t patch, int idxOfVar)
{
	assert(idxOfVar >= 0 && idxOfVar < varArr_getLength(patch->vars));

	return varArr_getElementHandle(patch->vars, idxOfVar);
}

extern void *
gridPatch_getVarDataHandle(const gridPatch_t patch, int idxOfVarData)
{
	void *data;

	assert(patch != NULL);
	assert(idxOfVarData >= 0
	       && idxOfVarData < varArr_getLength(patch->varData));

	data = varArr_getElementHandle(patch->varData, idxOfVarData);
	if (data == NULL)
		data = gridPatch_allocateVarData(patch, idxOfVarData);

	return data;
}

extern int
gridPatch_getNumVars(const gridPatch_t patch)
{
	assert(patch != NULL);

	return varArr_getLength(patch->vars);
}

extern void
gridPatch_transpose(gridPatch_t patch,
                    int         dimA,
                    int         dimB)
{
	uint32_t tmp;

	assert(patch != NULL);
	assert(dimA >= 0 && dimA < NDIM);
	assert(dimB >= 0 && dimB < NDIM);

#ifdef _OPENMP
#  pragma omp parallel for shared(patch, dimA, dimB)
#endif
	for (int i = 0; i < varArr_getLength(patch->vars); i++) {
		local_transposeVar(patch, i, dimA, dimB);
	}

	tmp                = patch->idxLo[dimA];
	patch->idxLo[dimA] = patch->idxLo[dimB];
	patch->idxLo[dimB] = tmp;
	tmp                = patch->dims[dimA];
	patch->dims[dimA]  = patch->dims[dimB];
	patch->dims[dimB]  = tmp;
}

extern void *
gridPatch_getWindowedDataCopy(gridPatch_t       patch,
                              int               idxVar,
                              gridPointUint32_t idxLo,
                              gridPointUint32_t idxHi,
                              uint64_t          *numElements)
{
	void              *data, *dataCopy;
	gridPointUint32_t dimsWindow;
	uint64_t          num = 1;
	gridVar_t         var;
	size_t            sizePerElement;
	size_t            offsetCopy = 0;
	size_t            offsetData = 0;

	// XXX Does not work with actualDim yet!

	assert(patch != NULL);
	assert((idxVar >= 0) && (idxVar < gridPatch_getNumVars(patch)));
	assert(idxLo[0] >= patch->idxLo[0]);
	assert(idxHi[0] < patch->idxLo[0] + patch->dims[0]);
	assert(idxLo[1] >= patch->idxLo[1]);
	assert(idxHi[1] < patch->idxLo[1] + patch->dims[1]);
#if (NDIM > 2)
	assert(idxLo[2] >= patch->idxLo[2]);
	assert(idxHi[2] < patch->idxLo[2] + patch->dims[2]);
#endif

	local_getWindowDims(idxLo, idxHi, dimsWindow, &num);

	var            = gridPatch_getVarHandle(patch, idxVar);
	data           = gridPatch_getVarDataHandle(patch, idxVar);
	dataCopy       = gridVar_getMemory(var, num);
	sizePerElement = gridVar_getSizePerElement(var);

#if (NDIM == 2)
	offsetData = idxLo[0] - patch->idxLo[0]
	             + (idxLo[1] - patch->idxLo[1]) * patch->dims[0];
	for (uint32_t j = 0; j < dimsWindow[1]; j++) {
		memcpy(((char *)dataCopy) + offsetCopy * sizePerElement,
		       ((char *)data) + offsetData * sizePerElement,
		       dimsWindow[0] * sizePerElement);
		offsetCopy += dimsWindow[0];
		offsetData += patch->dims[0];
	}
#elif (NDIM == 3)
	for (uint32_t k = 0; k < dimsWindow[2]; k++) {
		offsetData = idxLo[0] - patch->idxLo[0]
		             + (idxLo[1] - patch->idxLo[1]) * patch->dims[0]
		             + (idxLo[2] - patch->idxLo[2] + k)
		             * patch->dims[0] * patch->dims[1];
		for (uint32_t j = 0; j < dimsWindow[1]; j++) {
			memcpy(((char *)dataCopy) + offsetCopy * sizePerElement,
			       ((char *)data) + offsetData * sizePerElement,
			       dimsWindow[0] * sizePerElement);
			offsetCopy += dimsWindow[0];
			offsetData += patch->dims[0];
		}
	}
#endif

	if (numElements != NULL)
		*numElements = num;

	return dataCopy;
} /* gridPatch_getWindowedDataCopy */

extern void
gridPatch_putWindowedData(gridPatch_t       patch,
                          int               idxVar,
                          gridPointUint32_t idxLo,
                          gridPointUint32_t idxHi,
                          void              *data)
{
	void              *dataTarget;
	gridPointUint32_t windowDims;
	uint64_t          numCells;
	gridVar_t         var;
	size_t            sizePerElement;
	size_t            offsetTarget = 0;
	size_t            offsetData   = 0;

	assert(patch != NULL);
	assert(idxVar >= 0 && idxVar < gridPatch_getNumVars(patch));
	assert(idxLo[0] >= patch->idxLo[0]);
	assert(idxHi[0] < patch->idxLo[0] + patch->dims[0]);
	assert(idxLo[1] >= patch->idxLo[1]);
	assert(idxHi[1] < patch->idxLo[1] + patch->dims[1]);
#if (NDIM > 2)
	assert(idxLo[2] >= patch->idxLo[2]);
	assert(idxHi[2] < patch->idxLo[2] + patch->dims[2]);
#endif

	local_getWindowDims(idxLo, idxHi, windowDims, &numCells);

	var            = gridPatch_getVarHandle(patch, idxVar);
	dataTarget     = gridPatch_getVarDataHandle(patch, idxVar);
	sizePerElement = gridVar_getSizePerElement(var);

#if (NDIM == 2)
	offsetTarget = idxLo[0] - patch->idxLo[0]
	               + (idxLo[1] - patch->idxLo[1]) * patch->dims[0];
	for (uint32_t j = 0; j < windowDims[1]; j++) {
		memcpy(((char *)dataTarget) + offsetTarget * sizePerElement,
		       ((char *)data) + offsetData * sizePerElement,
		       windowDims[0] * sizePerElement);
		offsetData   += windowDims[0];
		offsetTarget += patch->dims[0];
	}
#elif (NDIM == 3)
	for (uint32_t k = 0; k < windowDims[2]; k++) {
		offsetTarget = idxLo[0] - patch->idxLo[0]
		               + (idxLo[1] - patch->idxLo[1]) * patch->dims[0]
		               + (idxLo[2] - patch->idxLo[2] + k)
		               * patch->dims[0] * patch->dims[1];
		for (uint32_t j = 0; j < windowDims[1]; j++) {
			memcpy(((char *)dataTarget) + offsetTarget * sizePerElement,
			       ((char *)data) + offsetData * sizePerElement,
			       windowDims[0] * sizePerElement);
			offsetData   += windowDims[0];
			offsetTarget += patch->dims[0];
		}
	}
#endif
} /* gridPatch_putWindowedData */

/*--- Implementations of local functions --------------------------------*/

static void
local_transposeVar(gridPatch_t patch,
                   int         idxOfVarData,
                   int         dimA,
                   int         dimB)
{
	void              *data;
	gridVar_t         var;
	int               size;
	gridPointUint32_t dimsT;
	uint32_t          tmp;
	uint64_t          numCellsActual;
	void              *dataT;

	data           = gridPatch_getVarDataHandle(patch, idxOfVarData);
	var            = gridPatch_getVarHandle(patch, idxOfVarData);
	size           = gridVar_getSizePerElement(var);
	gridPatch_getDimsActual(patch, idxOfVarData, dimsT);
	tmp            = dimsT[dimA];
	dimsT[dimA]    = dimsT[dimB];
	dimsT[dimB]    = tmp;
	numCellsActual = gridPatch_getNumCellsActual(patch, idxOfVarData);
	dataT          = gridVar_getMemory(var, numCellsActual);

	switch (size) {
	default:
#if (NDIM == 2)
		local_transposeVar_2d(data, dataT, size, dimsT);
#elif (NDIM == 3)
		if (((dimA == 0) && (dimB == 1)) || ((dimA == 1) && (dimB == 0)))
			local_transposeVar102_3d(data, dataT, size, dimsT);
		else if (((dimA == 0) && (dimB == 2)) || ((dimA == 2) && (dimB == 0)))
			local_transposeVar210_3d(data, dataT, size, dimsT);
		else if (((dimA == 1) && (dimB == 2)) || ((dimA == 2) && (dimB == 1)))
			local_transposeVar021_3d(data, dataT, size, dimsT);
#endif
		break;
	}
	gridPatch_replaceVarData(patch, idxOfVarData, dataT);
} /* local_transposeVar */

#if (NDIM == 2)
static void
local_transposeVar_2d(const void              *data,
                      void                    *dataT,
                      const int               size,
                      const gridPointUint32_t dimsT)
{
	// Write contiguous, read random
#  ifdef _OPENMP
#    pragma omp parallel for shared(data, dataT)
#  endif
	for (int k1 = 0; k1 < dimsT[1]; k1++) {
		for (int k0 = 0; k0 < dimsT[0]; k0++) {
			size_t posT = (k0 + k1 * dimsT[0]) * size;
			size_t pos  = (k1 + k0 * dimsT[1]) * size;
			memcpy(((char *)dataT) + posT, ((const char *)data) + pos, size);
		}
	}
}

#elif (NDIM == 3)
static void
local_transposeVar102_3d(const void              *data,
                         void                    *dataT,
                         const int               size,
                         const gridPointUint32_t dimsT)
{
	size_t pos, posT;

#  ifdef _OPENMP
#    pragma omp parallel for shared(data, dataT) private(pos, posT)
#  endif
	for (uint32_t k2 = 0; k2 < dimsT[2]; k2++) {
		for (uint32_t k1 = 0; k1 < dimsT[1]; k1++) {
			for (uint32_t k0 = 0; k0 < dimsT[0]; k0++) {
				posT = (k0 + (k1 + k2 * dimsT[1]) * dimsT[0]) * size;
				pos  = (k1 + (k0 + k2 * dimsT[0]) * dimsT[1]) * size;
				memcpy(((char *)dataT) + posT,
				       ((const char *)data) + pos,
				       size);
			}
		}
	}
}

static void
local_transposeVar210_3d(const void              *data,
                         void                    *dataT,
                         const int               size,
                         const gridPointUint32_t dimsT)
{
	size_t pos, posT;

#  ifdef _OPENMP
#    pragma omp parallel for shared(data, dataT) private(pos, posT)
#  endif
	for (uint32_t k2 = 0; k2 < dimsT[2]; k2++) {
		for (uint32_t k1 = 0; k1 < dimsT[1]; k1++) {
			for (uint32_t k0 = 0; k0 < dimsT[0]; k0++) {
				posT = (k0 + (k1 + k2 * dimsT[1]) * dimsT[0]) * size;
				pos  = (k2 + (k1 + k0 * dimsT[1]) * dimsT[2]) * size;
				memcpy(((char *)dataT) + posT,
				       ((const char *)data) + pos,
				       size);
			}
		}
	}
}

static void
local_transposeVar021_3d(const void              *data,
                         void                    *dataT,
                         const int               size,
                         const gridPointUint32_t dimsT)
{
	size_t pos, posT;

#  ifdef _OPENMP
#    pragma omp parallel for shared(data, dataT) private(pos, posT)
#  endif
	for (uint32_t k2 = 0; k2 < dimsT[2]; k2++) {
		for (uint32_t k1 = 0; k1 < dimsT[1]; k1++) {
			posT = ((k1 + k2 * dimsT[1]) * dimsT[0]) * size;
			pos  = ((k2 + k1 * dimsT[2]) * dimsT[0]) * size;
			memcpy(((char *)dataT) + posT,
			       ((const char *)data) + pos,
			       size * dimsT[0]);
		}
	}
}

#endif

static inline void
local_getWindowDims(gridPointUint32_t idxLo,
                    gridPointUint32_t idxHi,
                    gridPointUint32_t windowDims,
                    uint64_t          *numCells)
{
	uint64_t num = UINT64_C(1);

	for (int i = 0; i < NDIM; i++) {
		windowDims[i] = idxHi[i] - idxLo[i] + 1;
		num          *= windowDims[i];
	}

	if (numCells != NULL)
		*numCells = num;
}
