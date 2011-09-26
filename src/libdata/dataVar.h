// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DATAVAR_H
#define DATAVAR_H


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataVarType.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#ifdef WITH_HDF5
#  include <hdf5.h>
#endif


/*--- ADT handle --------------------------------------------------------*/
typedef struct dataVar_struct *dataVar_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern dataVar_t
dataVar_new(const char *name, dataVarType_t type, int numComponents);

extern dataVar_t
dataVar_clone(const dataVar_t var);

extern void
dataVar_del(dataVar_t *var);

extern dataVar_t
dataVar_getRef(dataVar_t var);

extern size_t
dataVar_getSizePerElement(dataVar_t var);

extern int
dataVar_getNumComponents(dataVar_t var);

extern dataVarType_t
dataVar_getType(dataVar_t var);

extern char *
dataVar_getName(dataVar_t var);

extern void
dataVar_setMemFuncs(dataVar_t var,
                    void *(*mallocFunc)(size_t size),
                    void (*freeFunc)(void *ptr));

extern void *
dataVar_getMemory(dataVar_t var, uint64_t numElements);

extern void *
dataVar_getCopy(dataVar_t var, uint64_t numElements, const void *data);

extern void
dataVar_freeMemory(dataVar_t var, void *data);

extern void *
dataVar_getPointerByOffset(dataVar_t var, const void *base, uint64_t offset);

extern void
dataVar_setFFTWPadded(dataVar_t var);

extern void
dataVar_unsetFFTWPadded(dataVar_t var);

extern bool
dataVar_isFFTWPadded(dataVar_t var);

extern void
dataVar_setComplexified(dataVar_t var);

extern void
dataVar_unsetComplexified(dataVar_t var);

extern bool
dataVar_isComplexified(dataVar_t var);

extern void
dataVar_rename(dataVar_t var, const char *newName);


#ifdef WITH_MPI
extern MPI_Datatype
dataVar_getMPIDatatype(dataVar_t var);

extern int
dataVar_getMPICount(dataVar_t var, uint64_t numElements);

#endif

#ifdef WITH_HDF5
extern hid_t
dataVar_getHDF5Datatype(const dataVar_t var);

#endif

#endif
