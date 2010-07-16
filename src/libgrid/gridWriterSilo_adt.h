// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERSILO_ADT_H
#define GRIDWRITERSILO_ADT_H

/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#ifdef WITH_SILO
#  include "gridWriter_adt.h"
#  include <silo.h>
#  ifdef WITH_MPI
#    include <pmpio.h>
#  endif


/*--- ADT implementation ------------------------------------------------*/
struct gridWriterSilo_struct {
	GRIDWRITER_T_CONTENT;
	char          *prefix;
	int           dbType;
	int           numFiles;
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
