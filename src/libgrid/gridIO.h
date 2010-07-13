// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDIO_H
#define GRIDIO_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"


/*--- Exported types ----------------------------------------------------*/
typedef enum {
	IO_TYPE_BOV,
	IO_TYPE_SILO,
	IO_TYPE_UNKNOWN
} gridIO_type_t;

/*--- Prototypes of exported functions ----------------------------------*/
extern gridIO_type_t
gridIO_getTypeFromName(const char *name);

extern char *
gridIO_getNameFromType(gridIO_type_t type);

#endif
