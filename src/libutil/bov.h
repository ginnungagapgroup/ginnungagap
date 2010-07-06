// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BOV_H
#define BOV_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Typedefs ----------------------------------------------------------*/
typedef enum {
	BOV_FORMAT_BYTE,
	BOV_FORMAT_INT,
	BOV_FORMAT_FLOAT,
	BOV_FORMAT_DOUBLE
} bovFormat_t;

typedef enum {
	BOV_ENDIAN_LITTLE,
	BOV_ENDIAN_BIG
} bovEndian_t;

typedef enum {
	BOV_CENTERING_ZONAL,
	BOV_CENTERING_NODAL
} bovCentering_t;


/*--- ADT handle --------------------------------------------------------*/
typedef struct bov_struct *bov_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern bov_t
bov_new(void);

extern bov_t
bov_newFromFile(const char *fileName);

extern void
bov_del(bov_t *bov);


#endif
