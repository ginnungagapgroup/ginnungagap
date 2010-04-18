// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREGULARFFT_H
#define GRIDREGULARFFT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegular.h"
#include "gridRegularDistrib.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct gridRegularFFT_struct *gridRegularFFT_t;


/*--- Exported defines --------------------------------------------------*/
#define GRIDREGULARFFT_FORWARD  1
#define GRIDREGULARFFT_BACKWARD -1


/*--- Prototypes of exported functions ----------------------------------*/
extern gridRegularFFT_t
gridRegularFFT_new(gridRegular_t        grid,
                   gridRegularDistrib_t gridDistrib,
                   int                  idxFFTVar);

extern void
gridRegularFFT_del(gridRegularFFT_t *fft);

extern void *
gridRegularFFT_execute(gridRegularFFT_t fft, int direction);

#endif
