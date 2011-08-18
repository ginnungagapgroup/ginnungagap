// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LARE_H
#define LARE_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include <stdint.h>
#include "../libgrid/gridPoint.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct lare_struct *lare_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern lare_t
lare_new(gridPointUint32_t dims, uint32_t numElements);

extern void
lare_del(lare_t *lare);

extern uint32_t
lare_getNumElements(const lare_t lare);

extern void
lare_getDims(const lare_t lare, gridPointUint32_t dims);

extern void
lare_getElement(const lare_t      lare,
                gridPointUint32_t element,
                uint32_t          idxOfElement);

extern void
lare_setElement(lare_t                  lare,
                const gridPointUint32_t element,
                uint32_t                idxOfElement);

extern void
lare_addElement(lare_t                  lare,
                const gridPointUint32_t element);

#endif
