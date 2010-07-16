// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BOV_H
#define BOV_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include "endian.h"


/*--- Typedefs ----------------------------------------------------------*/
typedef enum {
	BOV_FORMAT_BYTE,
	BOV_FORMAT_INT,
	BOV_FORMAT_FLOAT,
	BOV_FORMAT_DOUBLE
} bovFormat_t;

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

extern double
bov_getTime(const bov_t bov);

extern char *
bov_getDataFileName(const bov_t bov);

extern void
bov_getDataSize(const bov_t bov, uint32_t *dataSize);

extern bovFormat_t
bov_getDataFormat(const bov_t bov);

extern char *
bov_getVarName(const bov_t bov);

extern endian_t
bov_getDataEndian(const bov_t bov);

extern bovCentering_t
bov_getCentering(const bov_t bov);

extern void
bov_getBrickOrigin(const bov_t bov, double *brickOrigin);

extern void
bov_getBrickSize(const bov_t bov, double *brickSize);

extern int
bov_getDataComponents(const bov_t bov);

extern void
bov_setTime(bov_t bov, const double time);

extern void
bov_setDataFileName(bov_t bov, const char *dataFileName);

extern void
bov_setDataSize(bov_t bov, const uint32_t *dataSize);

extern void
bov_setDataFormat(bov_t bov, const bovFormat_t format);

extern void
bov_setVarName(bov_t bov, const char *varName);

extern void
bov_setDataEndian(bov_t bov, const endian_t endian);

extern void
bov_setCentering(bov_t bov, const bovCentering_t centering);

extern void
bov_setBrickOrigin(bov_t bov, const double *brickOrigin);

extern void
bov_setBrickSize(bov_t bov, const double *brickSize);

extern void
bov_setDataComponents(bov_t bov, const int numComponents);

extern void
bov_read(bov_t       bov,
         void        *data,
         bovFormat_t dataFormat,
         int         numComponents);

extern void
bov_readWindowed(bov_t       bov,
                 void        *data,
                 bovFormat_t dataFormat,
                 int         numComponents,
                 uint32_t    *idxLo,
                 uint32_t    *dims);

#endif
