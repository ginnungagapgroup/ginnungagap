// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoTF.h"
#include "cosmoTFEisensteinHu.h"
#include <math.h>
#include <assert.h>
#include <inttypes.h>
#include "../libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern void
cosmoTF_eisensteinHu1998(double   omegaMatter0,
                         double   omegaBaryon0,
                         double   hubble,
                         double   tempCMB,
                         uint32_t numPoints,
                         double   *k,
                         double   *P)
{
	assert(isfinite(omegaMatter0));
	assert(isfinite(omegaBaryon0) && isgreaterequal(omegaBaryon0, 0.0));
	assert(isgreaterequal(omegaMatter0, omegaBaryon0));
	assert(isfinite(hubble) && hubble > 0.0);
	assert(isfinite(tempCMB) && tempCMB > 0.0);
	assert(numPoints > UINT32_C(0));
	assert(k != NULL);
	assert(P != NULL);

	TFfit_hmpc(omegaMatter0, omegaBaryon0 / omegaMatter0, hubble,
	           tempCMB, (int)numPoints, k, P, NULL, NULL);
}

/*--- Implementations of local functions --------------------------------*/
