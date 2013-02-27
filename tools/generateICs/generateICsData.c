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
#include "../../src/libutil/xmem.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functions -----------------------------*/
extern generateICsData_t
generateICsData_new(double       boxsizeInMpch,
                    double       aInit,
                    cosmoModel_t model)
{
	double            err;
	double            adot      = cosmoModel_calcADot(model, aInit);
	double            growthVel = cosmoModel_calcDlnGrowthDlna(model, aInit,
	                                                           &err);
	double            vFact     = 1.  / ( (adot) * 100. * (growthVel) );
	generateICsData_t data;

	data = xmalloc( sizeof(struct generateICsData_struct) );

	struct generateICsData_struct tmp
	    = { .boxsizeInMpch = boxsizeInMpch,
		    .aInit         = aInit,
		    .model         = model,
		    .vFact         = vFact,
		    .adot          = adot,
		    .growthVel     = growthVel,
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
