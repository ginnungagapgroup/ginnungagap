// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LAREIO_H
#define LAREIO_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include <stdbool.h>


/*--- Exported types ----------------------------------------------------*/
typedef enum {
	LAREIO_TYPE_LEGACY,
	LAREIO_TYPE_UNKNOWN
} lareIO_type_t;

/*--- Prototypes of exported functions ----------------------------------*/
extern lareIO_type_t
lareIO_getTypeFromName(const char *name);

extern const char *
lareIO_getNameFromType(lareIO_type_t type);

#endif
