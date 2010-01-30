// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "ginnungagap.h"
#include "ginnungagapConfig.h"
#include "rng.h"
#include "fft.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
struct ginnungagap_struct {
	ginnungagapConfig_t config;
	rng_t               rng;
	fft_t               fftHigh;
	fft_t               fftLow;
};


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern ginnungagap_t
ginnungagap_new(parse_ini_t ini)
{
	ginnungagap_t ginnungagap;
	assert(ini != NULL);

	ginnungagap         = xmalloc(sizeof(struct ginnungagap_struct));
	ginnungagap->config = ginnungagapConfig_new(ini);
	ginnungagap->rng    = rng_new(ini);
	ginnungagap->fftHigh = fft_new(ginnungagap->config->highDim1D);
	if (ginnungagap->config->replaceLowFreqModes)
		ginnungagap->fftLow = fft_new(ginnungagap->config->lowDim1D);
	else
		ginnungagap->fftLow = NULL;

	return ginnungagap;
}

extern void
ginnungagap_run(ginnungagap_t ginnungagap)
{
	assert(ginnungagap != NULL);
}

extern void
ginnungagap_del(ginnungagap_t *ginnungagap)
{
	assert(ginnungagap != NULL);
	assert(*ginnungagap != NULL);

	if ((*ginnungagap)->fftLow != NULL)
		fft_del(&((*ginnungagap)->fftLow));
	fft_del(&((*ginnungagap)->fftHigh));
	rng_del(&((*ginnungagap)->rng));
	ginnungagapConfig_del(&((*ginnungagap)->config));
	xfree(*ginnungagap);
	*ginnungagap = NULL;
}

/*--- Implementations of local functions --------------------------------*/
