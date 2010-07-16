// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITER_ADT_H
#define GRIDWRITER_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter.h"
#include <stdbool.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#include "gridIO.h"
#include "gridPatch.h"
#include "gridRegular.h"
#include "gridPoint.h"
#include "../libutil/parse_ini.h"


/*--- Internal structures -----------------------------------------------*/
struct gridWriter_func_struct {
	void (*del)(gridWriter_t *writer);
	void (*activate)(gridWriter_t writer);
	void (*deactivate)(gridWriter_t writer);
	void (*writeGridPatch)(gridWriter_t   writer,
	                       gridPatch_t    patch,
	                       const char     *patchName,
	                       gridPointDbl_t origin,
	                       gridPointDbl_t delta);
	void (*writeGridRegular)(gridWriter_t  writer,
	                         gridRegular_t grid);
#ifdef WITH_MPI
	void (*initParallel)(gridWriter_t writer, MPI_Comm mpiComm);
#endif
};

typedef struct gridWriter_func_struct *gridWriter_func_t;


/*--- ADT implementation ------------------------------------------------*/
#define GRIDWRITER_T_CONTENT \
    gridIO_type_t type;        \
    gridWriter_func_t func;    \
    bool              isActive;

struct gridWriter_struct {
	GRIDWRITER_T_CONTENT;
};


#endif
