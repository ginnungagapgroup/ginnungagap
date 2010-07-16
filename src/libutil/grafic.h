// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRAFIC_H
#define GRAFIC_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdint.h>
#include <stdbool.h>


/*--- Typedefs ----------------------------------------------------------*/
typedef enum {
	GRAFIC_FORMAT_FLOAT,
	GRAFIC_FORMAT_DOUBLE
} graficFormat_t;


/*--- ADT handle --------------------------------------------------------*/
typedef struct grafic_struct *grafic_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern grafic_t
grafic_new(bool isWhiteNoise);

extern grafic_t
grafic_newFromFile(const char *fileName, bool isWhiteNoise);

extern void
grafic_del(grafic_t *grafic);

extern const char *
grafic_getFileName(grafic_t grafic);

extern void
grafic_getSize(grafic_t grafic, uint32_t *np);

extern float
grafic_getDx(grafic_t grafic);

extern void
grafic_getXoff(grafic_t grafic, float *xoff);

extern float
grafic_getAstart(grafic_t grafic);

extern float
grafic_getOmegam(grafic_t grafic);

extern float
grafic_getOmegav(grafic_t grafic);

extern float
grafic_getH0(grafic_t grafic);

extern int
grafic_getIseed(grafic_t grafic);

extern void
grafic_setFileName(grafic_t grafic, const char *fileName);

extern void
grafic_setSize(grafic_t grafic, uint32_t *np);

extern void
grafic_setDx(grafic_t grafic, float dx);

extern void
grafic_setXoff(grafic_t grafic, float *xoff);

extern void
grafic_setAstart(grafic_t grafic, float astart);

extern void
grafic_setOmegam(grafic_t grafic, float omegam);

extern void
grafic_setOmegav(grafic_t grafic, float omegav);

extern void
grafic_setH0(grafic_t grafic, float h0);

extern void
grafic_setIseed(grafic_t grafic, int iseed);

extern void
grafic_read(grafic_t       grafic,
            void           *data,
            graficFormat_t dataFormat,
            int            numComponents);

extern void
grafic_write(grafic_t       grafic,
             void           *data,
             graficFormat_t dataFormat,
             int            numComponents);

extern void
grafic_readWindowed(grafic_t       grafic,
                    void           *data,
                    graficFormat_t dataFormat,
                    int            numComponents,
                    uint32_t       *idxLo,
                    uint32_t       *dims);

#endif
