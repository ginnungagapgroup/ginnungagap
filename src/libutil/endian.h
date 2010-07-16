// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ENDIAN_H
#define ENDIAN_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Typedefs ----------------------------------------------------------*/
typedef enum {
	ENDIAN_LITTLE,
	ENDIAN_BIG
} endian_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
endian_systemIsLittle(void);

extern bool
endian_systemIsBig(void);

extern endian_t
endian_getSystemEndianess(void);

extern bool
endian_fileIsLittleByBlock(const char *fname);

extern bool
endian_fileIsBigByBlock(const char *fname);

extern endian_t
endian_getFileEndianessByBlock(const char *fname);


#endif
