// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoTF.h"
#include "cosmoTFEisensteinHu.h"
#include <math.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include "../libutil/xmem.h"
#include "../libutil/parse_ini.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern cosmoTF_t
cosmoTF_getTypeFromIni(parse_ini_t ini, const char *sectionName)
{
	char      *tfTypeName = NULL;
	cosmoTF_t tfType      = COSMOTF_TYPE_EISENSTEINHU1998;

	assert(ini != NULL);
	assert(sectionName != NULL);

	getFromIni(&tfTypeName, parse_ini_get_string, ini,
	           "transferFunctionType", sectionName);

	if (strncmp(tfTypeName, "EisensteinHu1998", 16) == 0)
		tfType = COSMOTF_TYPE_EISENSTEINHU1998;
	else if (strncmp(tfTypeName, "ScaleFree", 9) == 0)
		tfType = COSMOTF_TYPE_SCALEFREE;

	xfree(tfTypeName);

	return tfType;
}

extern void
cosmoTF_eisensteinHu1998(double   omegaMatter0,
                         double   omegaBaryon0,
                         double   hubble,
                         double   tempCMB,
                         uint32_t numPoints,
                         double   *k,
                         double   *T)
{
	assert(isfinite(omegaMatter0));
	assert(isfinite(omegaBaryon0) && isgreaterequal(omegaBaryon0, 0.0));
	assert(isgreaterequal(omegaMatter0, omegaBaryon0));
	assert(isfinite(hubble) && hubble > 0.0);
	assert(isfinite(tempCMB) && tempCMB > 0.0);
	assert(numPoints > UINT32_C(0));
	assert(k != NULL);
	assert(T != NULL);

	TFfit_hmpc(omegaMatter0, omegaBaryon0 / omegaMatter0, hubble,
	           tempCMB, (int)numPoints, k, T, NULL, NULL);
}

extern void
cosmoTF_scaleFree(uint32_t numPoints,
                  double   *k,
                  double   *T)
{
	assert(numPoints > UINT32_C(0));
	assert(k != NULL);
	assert(isfinite(k[0]) && isfinite(k[numPoints - 1]));
	assert(0.0 < k[0] && k[0] < k[numPoints - 1]);
	assert(T != NULL);

	for (uint32_t i = 0; i < numPoints; i++)
		T[i] = 1.0;
}

/*--- Implementations of local functions --------------------------------*/
