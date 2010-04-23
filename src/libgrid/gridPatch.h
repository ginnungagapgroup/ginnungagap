// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPATCH_H
#define GRIDPATCH_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "gridVar.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridPatch_struct *gridPatch_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridPatch_t
gridPatch_new(gridPointUint32_t idxLo, gridPointUint32_t idxHi);

extern void
gridPatch_del(gridPatch_t *gridPatch);

extern uint32_t
gridPatch_getOneDim(gridPatch_t patch, int idxOfDim);

extern uint32_t
gridPatch_getDimActual1D(gridPatch_t patch, int idxOfVar, int dim);

extern void
gridPatch_getDims(gridPatch_t patch, gridPointUint32_t dim);

extern void
gridPatch_getDimsActual(gridPatch_t       patch,
                        int               idxOfVar,
                        gridPointUint32_t dimsActual);

extern uint64_t
gridPatch_getNumCells(gridPatch_t patch);

extern uint64_t
gridPatch_getNumCellsActual(gridPatch_t patch, int idxOfVar);

extern void
gridPatch_getIdxLo(gridPatch_t patch, gridPointUint32_t idxLo);

extern int
gridPatch_attachVarData(gridPatch_t patch, gridVar_t var);

extern void *
gridPatch_detachVarData(gridPatch_t patch, int idxOfVarData);

extern void
gridPatch_replaceVarData(gridPatch_t patch, int idxOfVarData, void *newData);

extern gridVar_t
gridPatch_getVarHandle(gridPatch_t patch, int idxOfVar);

extern void *
gridPatch_getVarDataHandle(gridPatch_t patch, int idxOfVarData);

extern int
gridPatch_getNumVars(gridPatch_t patch);

extern void
gridPatch_transposeVar(gridPatch_t patch,
                       int         idxOfVarData,
                       int         dimA,
                       int         dimB);

extern void *
gridPatch_getWindowedDataCopy(gridPatch_t       patch,
                              int               idxVar,
                              gridPointUint32_t idxLo,
                              gridPointUint32_t idxHi,
                              uint64_t          *numElements);

#endif
