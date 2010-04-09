// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GINNUNGAGAPCONFIG_H
#define GINNUNGAGAPCONFIG_H


/*--- Includes ----------------------------------------------------------*/
#include "../libutil/parse_ini.h"
#include <stdint.h>
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct ginnungagapConfig_struct *ginnungagapConfig_t;


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagapConfig_struct {
	uint32_t dim1D;
	double   boxsizeInMpch;
	char     *gridName;
	bool     useConstraints;
	char     *fileNameConstraints;
	uint32_t dim1DConstraints;
#ifdef WITH_SILO
	char     *filePrefix;
	int      dbtype;
#  ifdef WITH_MPI
	int      numFiles;
#  endif
#endif
};


/*--- Prototypes of exported functions ----------------------------------*/
extern ginnungagapConfig_t
ginnungagapConfig_new(parse_ini_t ini);

extern void
ginnungagapConfig_del(ginnungagapConfig_t *config);


#endif
