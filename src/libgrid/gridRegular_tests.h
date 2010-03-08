// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARTESTS_H
#define GRIDREGULARTESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
gridRegular_newWithoutData_test(void);

extern bool
gridRegular_del_test(void);

extern bool
gridRegular_setOrigin_test(void);

extern bool
gridRegular_setExtent_test(void);

extern bool
gridRegular_attachVar_test(void);

extern bool
gridRegular_allocVar_test(void);

extern bool
gridRegular_deallocVar_test(void);

extern bool
gridRegular_getNumVars_test(void);


#ifdef WITH_MPI
extern bool
gridRegular_mpiSetDistribution_test(void);

#endif


#ifdef WITH_SILO
extern bool
gridRegular_writeSilo_test(void);

#endif


#endif
