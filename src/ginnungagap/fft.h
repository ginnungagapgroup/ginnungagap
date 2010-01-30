// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef FFT_H
#define FFT_H


/*--- Includes ----------------------------------------------------------*/
#include <stdint.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct fft_struct *fft_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern fft_t
fft_new(uint32_t dim);

extern void
fft_del(fft_t *fft);


#endif
