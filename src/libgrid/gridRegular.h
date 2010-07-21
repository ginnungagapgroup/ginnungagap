// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULAR_H
#define GRIDREGULAR_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridPoint.h"
#include "gridVar.h"
#include "gridPatch.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridRegular_struct *gridRegular_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridRegular_t
gridRegular_new(const char        *name,
                gridPointDbl_t    origin,
                gridPointDbl_t    extent,
                gridPointUint32_t dims);

extern void
gridRegular_del(gridRegular_t *gridRegular);

extern gridRegular_t
gridRegular_getRef(gridRegular_t grid);

extern char *
gridRegular_getName(gridRegular_t grid);

extern void
gridRegular_getOrigin(gridRegular_t grid, gridPointDbl_t origin);

extern void
gridRegular_getExtent(gridRegular_t grid, gridPointDbl_t extent);

extern void
gridRegular_getDelta(gridRegular_t grid, gridPointDbl_t delta);

extern void
gridRegular_getDims(gridRegular_t grid, gridPointUint32_t dims);

extern void
gridRegular_getDimsComplex(gridRegular_t grid, gridPointUint32_t dimsComplex);

extern void
gridRegular_getPermute(gridRegular_t grid, gridPointInt_t permute);

extern int
gridRegular_getNumVars(gridRegular_t grid);

extern uint64_t
gridRegular_getNumCellsTotal(gridRegular_t grid);

extern void
gridRegular_setComplexified(gridRegular_t grid, int idxOfVar);

extern void
gridRegular_unsetComplexified(gridRegular_t grid, int idxOfVar);

extern bool
gridRegular_isComplexified(gridRegular_t grid, int idxOfVar);

extern int
gridRegular_attachVar(gridRegular_t grid, gridVar_t var);

extern gridVar_t
gridRegular_detachVar(gridRegular_t grid, int idxOfVar);

extern gridVar_t
gridRegular_getVarHandle(gridRegular_t grid, int idxOfVar);

extern void
gridRegular_freeVarData(gridRegular_t grid, int idxOfVarData);

extern int
gridRegular_getNumPatches(gridRegular_t grid);

extern int
gridRegular_attachPatch(gridRegular_t grid, gridPatch_t patch);

extern gridPatch_t
gridRegular_detachPatch(gridRegular_t grid, int idxOfPatch);

extern gridPatch_t
gridRegular_getPatchHandle(gridRegular_t grid, int idxPatchToGet);

extern void
gridRegular_replacePatch(gridRegular_t grid,
                         int           idxOfPatch,
                         gridPatch_t   newPatch);

extern void
gridRegular_transpose(gridRegular_t grid, int dimA, int dimB);

#endif
