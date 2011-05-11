// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDPATCH_H
#define GRIDPATCH_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "../libdata/dataVar.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridPatch_struct *gridPatch_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridPatch_t
gridPatch_new(gridPointUint32_t idxLo, gridPointUint32_t idxHi);

extern void
gridPatch_del(gridPatch_t *gridPatch);

extern uint32_t
gridPatch_getOneDim(const gridPatch_t patch, int idxOfDim);

extern uint32_t
gridPatch_getDimActual1D(const gridPatch_t patch, int idxOfVar, int dim);

extern void
gridPatch_getDims(const gridPatch_t patch, gridPointUint32_t dim);

extern void
gridPatch_getDimsActual(const gridPatch_t patch,
                        int               idxOfVar,
                        gridPointUint32_t dimsActual);

extern uint64_t
gridPatch_getNumCells(const gridPatch_t patch);

extern uint64_t
gridPatch_getNumCellsActual(const gridPatch_t patch, int idxOfVar);

extern void
gridPatch_getIdxLo(const gridPatch_t patch, gridPointUint32_t idxLo);

extern int
gridPatch_attachVar(gridPatch_t patch, dataVar_t var);

extern dataVar_t
gridPatch_detachVar(gridPatch_t patch, int idxOfVar);

extern void *
gridPatch_allocateVarData(gridPatch_t patch, int idxOfVarData);

extern void
gridPatch_freeVarData(gridPatch_t patch, int idxOfVarData);

extern void
gridPatch_replaceVarData(gridPatch_t patch, int idxOfVarData, void *newData);

extern dataVar_t
gridPatch_getVarHandle(const gridPatch_t patch, int idxOfVar);

extern void *
gridPatch_getVarDataHandle(const gridPatch_t patch, int idxOfVarData);

extern int
gridPatch_getNumVars(gridPatch_t patch);

extern void
gridPatch_transpose(gridPatch_t patch,
                    int         dimA,
                    int         dimB);

extern void *
gridPatch_getWindowedDataCopy(gridPatch_t       patch,
                              int               idxVar,
                              gridPointUint32_t idxLo,
                              gridPointUint32_t idxHi,
                              uint64_t          *numElements);

extern void
gridPatch_putWindowedData(gridPatch_t       patch,
                          int               idxVar,
                          gridPointUint32_t idxLo,
                          gridPointUint32_t idxHi,
                          void              *data);

#endif
