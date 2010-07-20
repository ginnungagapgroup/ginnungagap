// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDWRITERGRAFIC_ADT_H
#define GRIDWRITERGRAFIC_ADT_H

/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridWriter_adt.h"
#include <stdbool.h>
#ifdef WITH_MPI
#  include <mpi.h>
#  include <pmpio.h>
#endif
#include "../libutil/grafic.h"


/*--- ADT implementation ------------------------------------------------*/
struct gridWriterGrafic_struct {
	GRIDWRITER_T_CONTENT
	grafic_t      grafic;
	int           numOutFiles;
	int           curOutFile;
	char          **fileNames;
#ifdef WITH_MPI
	PMPIO_baton_t *baton;
	int           groupRank;
	int           rankInGroup;
	int           globalRank;
#endif
};

#endif
