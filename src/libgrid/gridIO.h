// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDIO_H
#define GRIDIO_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include <stdbool.h>


/*--- Exported types ----------------------------------------------------*/
typedef enum {
	GRIDIO_TYPE_BOV,
	GRIDIO_TYPE_SILO,
	GRIDIO_TYPE_GRAFIC,
	GRIDIO_TYPE_UNKNOWN
} gridIO_type_t;

/*--- Prototypes of exported functions ----------------------------------*/
extern gridIO_type_t
gridIO_getTypeFromName(const char *name);

extern const char *
gridIO_getNameFromType(gridIO_type_t type);

#endif
