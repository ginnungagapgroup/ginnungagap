// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDVAR_H
#define GRIDVAR_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarType.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridVar_struct *gridVar_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridVar_t
gridVar_new(const char *name, gridVarType_t type, int numComponents);

extern gridVar_t
gridVar_clone(const gridVar_t var);

extern void
gridVar_del(gridVar_t *var);

extern gridVar_t
gridVar_getRef(gridVar_t var);

extern size_t
gridVar_getSizePerElement(gridVar_t var);

extern int
gridVar_getNumComponents(gridVar_t var);

extern gridVarType_t
gridVar_getType(gridVar_t var);

extern char *
gridVar_getName(gridVar_t var);

extern void
gridVar_setMemFuncs(gridVar_t var,
                    void *(*mallocFunc)(size_t size),
                    void      (*freeFunc)(void *ptr));

extern void *
gridVar_getMemory(gridVar_t var, uint64_t numElements);

extern void
gridVar_freeMemory(gridVar_t var, void *data);

extern void *
gridVar_getPointerByOffset(gridVar_t var, const void *base, uint64_t offset);

extern void
gridVar_setFFTWPadded(gridVar_t var);

extern void
gridVar_unsetFFTWPadded(gridVar_t var);

extern bool
gridVar_isFFTWPadded(gridVar_t var);

extern void
gridVar_setComplexified(gridVar_t var);

extern void
gridVar_unsetComplexified(gridVar_t var);

extern bool
gridVar_isComplexified(gridVar_t var);


#ifdef WITH_MPI
extern MPI_Datatype
gridVar_getMPIDatatype(gridVar_t var);

extern int
gridVar_getMPICount(gridVar_t var, uint64_t numElements);

#endif

#endif
