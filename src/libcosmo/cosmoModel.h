// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.

#ifndef COSMOMODEL_H
#define COSMOMODEL_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct cosmoModel_struct *cosmoModel_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern cosmoModel_t
cosmoModel_newFromFile(const char *fname);

extern void
cosmoModel_del(cosmoModel_t *model);

extern double
cosmoModel_calcAgeFromExpansion(cosmoModel_t model,
                                double a,
                                double *error);

extern double
cosmoModel_calcExpansionFromAge(cosmoModel_t model,
                                double t,
                                double *error);

extern double
cosmoModel_calcADot(cosmoModel_t model, double a);

extern double
cosmoModel_calcHubble(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaRad(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaMatter(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaBaryon(cosmoModel_t model, double a);

extern double
cosmoModel_calcOmegaLambda(cosmoModel_t model, double a);

extern double
cosmoModel_calcGrowth(cosmoModel_t model, double a, double *error);


#endif
