// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef VARARR_H
#define VARARR_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Exported defines --------------------------------------------------*/


/*--- ADT handle --------------------------------------------------------*/
typedef struct varArr_struct *varArr_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern varArr_t
varArr_new(int numElements);

extern void
varArr_del(varArr_t *arr);

extern int
varArr_getLength(varArr_t arr);

extern int
varArr_insert(varArr_t arr, void *element);

extern void *
varArr_remove(varArr_t arr, int numElement);

extern void *
varArr_replace(varArr_t arr, int numElement, void *newElement);

extern void *
varArr_getElementHandle(varArr_t arr, int numElement);

#endif
