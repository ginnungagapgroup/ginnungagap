// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDSLAB_H
#define GRIDSLAB_H


/*--- Includes ----------------------------------------------------------*/
#include "grid_config.h"
#ifdef WITH_SILO
#  include <silo.h>
#endif
#include <stdint.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridSlab_struct *gridSlab_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern gridSlab_t
gridSlab_newWithoutData(uint32_t coordStart, uint32_t thicknessInCells);

extern void
gridSlab_allocateData(gridSlab_t slab, uint64_t numTotalCells);

extern void
gridSlab_addDataReference(gridSlab_t slab,
                          fpv_t      *data,
                          uint64_t   numTotalCells);

extern void
gridSlab_setFreeDataOnDel(gridSlab_t slab, bool choice);

#ifdef WITH_SILO
extern void
gridSlab_writeSilo(gridSlab_t slab,
                   DBfile     *f,
                   const char *meshname,
                   const char *varname,
                   int        ndims,
                   const int  *dims);
#endif

extern void
gridSlab_del(gridSlab_t *slab);


#endif
