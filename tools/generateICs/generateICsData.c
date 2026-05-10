// Copyright (C) 2013, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file generateICs/generateICsData.c
 * @ingroup  toolsGICSData
 * @brief  Implements data related functions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "generateICsConfig.h"
#include "generateICsData.h"
#include <assert.h>
#include <string.h>
#include <math.h>
#include "../../src/libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern generateICsData_t
generateICsData_new(double       boxsizeInMpch,
                    double       aInit,
                    cosmoModel_t model,
                    bool         do2LPT)
{
	double            err;
	double            adot      = cosmoModel_calcADot(model, aInit);
	double            growthVel = cosmoModel_calcDlnGrowthDlna(model, aInit,
	                                                           &err);
	double            vFact     = 1.  / ( (adot) * 100. * (growthVel) );
	double            vFact2    = 0.0;
	generateICsData_t data;

	if (do2LPT) {
		double growthVel2 = cosmoModel_calcDlnGrowthDlna2lpt(model, aInit,
		                                                     &err);
		double omegaM     = cosmoModel_calcOmegaMatter(model, aInit);
		double D2ratio    = -(3. / 7.) * pow(omegaM, -1. / 143.);
		vFact2 = 1. / (adot * 100. * growthVel2 * D2ratio);
	}

	data = xmalloc( sizeof(struct generateICsData_struct) );

	struct generateICsData_struct tmp
	    = { .boxsizeInMpch = boxsizeInMpch,
		    .aInit         = aInit,
		    .model         = model,
		    .vFact         = vFact,
		    .adot          = adot,
		    .growthVel     = growthVel,
		    .do2LPT        = do2LPT,
		    .vFact2        = vFact2,
		    .posFactor     = 1.0,
		    .velFactor     = 1.0 };

	memcpy( data, &tmp, sizeof(struct generateICsData_struct) );

	return data;
} // generateICsData_new

extern void
generateICsData_del(generateICsData_t *data)
{
	assert(data != NULL && *data != NULL);

	cosmoModel_t tmp = (*data)->model;
	cosmoModel_del(&tmp);
	xfree(*data);

	*data = NULL;
}

/*--- Implementations of local functions --------------------------------*/
