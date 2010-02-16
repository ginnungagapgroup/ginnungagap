// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "fft.h"
#include "../libutil/xmem.h"
#include <assert.h>
#ifdef WITH_MPI
#  include <mpi.h>
#endif
#if (WITH_FFTW == 2)
#  ifdef WITH_MPI
#    ifdef ENABLE_DOUBLE
#      include <drfftw_mpi.h>
#    else
#      include <srfftw_mpi.h>
#    endif
#  else
#    ifdef ENABLE_DOUBLE
#      include <drfftw.h>
#    else
#      include <srfftw.h>
#    endif
#  endif
#else
#  ifdef WITH_MPI
#    include <fftw3-mpi.h>
#  else
#    include <fftw3.h>
#  endif
#endif


/*--- Implemention of main structure ------------------------------------*/
struct fft_struct {
	int              nx, ny, nz;
#if (WITH_FFTW == 2)
	int              local_nx;
	int              local_xStart;
	int              local_nyAfterTranspose;
	int              local_yStartAfterTranspose;
	int              local_totalLocalSize;
#  ifdef WITH_MPI
	rfftwnd_mpi_plan plan, iplan;
#  else
	rfftwnd_plan     plan, iplan;
#  endif
	fftw_real        *data, *work;
	fftw_complex     *cdata;
#else
	// TODO Implement usage of FFTW3.
#endif
};


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/
static void
local_createPlans(fft_t fft);

static void
local_calculateOffsets(fft_t fft);

static void
local_destroyPlans(fft_t fft);


/*--- Implementations of exported functios ------------------------------*/
extern fft_t
fft_new(uint32_t dim)
{
	fft_t fft;

	fft     = xmalloc(sizeof(struct fft_struct));

	fft->nx = (int)dim;
	fft->ny = (int)dim;
	fft->nz = (int)dim;

	local_createPlans(fft);
	local_calculateOffsets(fft);

	fft->data  = xmalloc(sizeof(fftw_real) * fft->local_totalLocalSize);
	fft->work  = xmalloc(sizeof(fftw_real) * fft->local_totalLocalSize);
	fft->cdata = NULL;

	return fft;
}

extern void
fft_del(fft_t *fft)
{
	assert(fft != NULL);
	assert(*fft != NULL);

	if ((*fft)->data != NULL)
		xfree((*fft)->data);
	if ((*fft)->work != NULL)
		xfree((*fft)->work);
	local_destroyPlans(*fft);

	xfree(*fft);
	*fft = NULL;
}

/*--- Implementations of local functions --------------------------------*/
static void
local_createPlans(fft_t fft)
{
#if (WITH_FFTW == 2)
#  ifdef WITH_MPI
	fft->plan = rfftw3d_mpi_create_plan(MPI_COMM_WORLD,
	                                    fft->nx, fft->ny, fft->nz,
	                                    FFTW_REAL_TO_COMPLEX,
	                                    FFTW_ESTIMATE);
	fft->iplan = rfftw3d_mpi_create_plan(MPI_COMM_WORLD,
	                                     fft->nx, fft->ny, fft->nz,
	                                     FFTW_COMPLEX_TO_REAL,
	                                     FFTW_ESTIMATE);
#  else
	fft->plan  = rfftw3d_create_plan(fft->nx, fft->ny, fft->nz,
	                                 FFTW_REAL_TO_COMPLEX, FFTW_ESTIMATE);
	fft->iplan = rfftw3d_create_plan(fft->nx, fft->ny, fft->nz,
	                                 FFTW_COMPLEX_TO_REAL, FFTW_ESTIMATE);
#  endif
#else
	// TODO Implement usage of FFTW3.
#endif
}

static void
local_calculateOffsets(fft_t fft)
{
#if (WITH_FFTW == 2)
#  ifdef WITH_MPI
	rfftwnd_mpi_local_sizes(fft->plan, &(fft->local_nx),
	                        &(fft->local_xStart),
	                        &(fft->local_nyAfterTranspose),
	                        &(fft->local_yStartAfterTranspose),
	                        &(fft->local_totalLocalSize));
#  else
	fft->local_nx                   = fft->nx;
	fft->local_xStart               = 0;
	fft->local_nyAfterTranspose     = fft->ny;
	fft->local_yStartAfterTranspose = 0;
	fft->local_totalLocalSize       = fft->nx * fft->ny * 2
	                                  * (fft->nz / 2 + 1);
#  endif
#else
	// TODO Implement usage of FFTW3.
#endif
}

static void
local_destroyPlans(fft_t fft)
{
#if (WITH_FFTW==2)
#ifdef WITH_MPI
	rfftwnd_mpi_destroy_plan(fft->plan);
	rfftwnd_mpi_destroy_plan(fft->iplan);
#else
	rfftwnd_destroy_plan(fft->plan);
	rfftwnd_destroy_plan(fft->iplan);
#endif
#else
	// TODO Implement usage of FFTW3.
#endif
}
