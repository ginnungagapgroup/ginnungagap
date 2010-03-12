// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERSILO_ADT_H
#define GRIDWRITERSILO_ADT_H

/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#ifdef WITH_SILO
#  include <stdbool.h>
#  ifdef WITH_MPI
#    include <mpi.h>
#    include <pmpio.h>
#  endif


/*--- ADT implementation ------------------------------------------------*/
struct gridWriterSilo_struct {
	char          *prefix;
	int           dbType;
	bool          isActive;
	DBfile        *f;
	char          *fileName;
	char          *dirName;
#  ifdef WITH_MPI
	PMPIO_baton_t *baton;
	int           groupRank;
	int           rankInGroup;
	int           globalRank;
#  endif
};

#endif
#endif
