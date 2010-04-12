// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridRegularFFT.h"
#include <assert.h>
#include "../libutil/xmem.h"


/*--- Implemention of main structure ------------------------------------*/
#include "gridRegularFFT_adt.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern gridRegularFFT_t
gridRegularFFT_new(gridRegular_t        grid,
                   gridRegularDistrib_t gridDistrib,
                   int                  idxFFTVar)
{
	gridRegularFFT_t fft;

	assert(grid != NULL);
	assert(gridDistrib != NULL);
	assert(idxFFTVar >= 0 && idxFFTVar < gridRegular_getNumVars(grid));

	fft              = xmalloc(sizeof(struct gridRegularFFT_struct));
	fft->grid        = gridRegular_getRef(grid);
	fft->gridDistrib = gridRegularDistrib_getRef(gridDistrib);
	fft->idxFFTVar   = idxFFTVar;

	return fft;
}

extern void
gridRegularFFT_del(gridRegularFFT_t *fft)
{
	assert(fft != NULL && *fft != NULL);

	gridRegular_del(&((*fft)->grid));
	gridRegularDistrib_del(&((*fft)->gridDistrib));
	xfree(*fft);

	*fft = NULL;
}

extern void
gridRegularFFT_execute(gridRegularFFT_t fft, int direction)
{
	assert(fft != NULL);
	assert(direction != 0);
}

/*--- Implementations of local functions --------------------------------*/
