// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOMODEL_H
#define COSMOMODEL_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct cosmoModel_struct *cosmoModel_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern cosmoModel_t
cosmoModel_new(void);

extern cosmoModel_t
cosmoModel_newFromFile(const char *fname);

extern cosmoModel_t
cosmoModel_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
cosmoModel_del(cosmoModel_t *model);

extern double
cosmoModel_getOmegaRad0(const cosmoModel_t model);

extern double
cosmoModel_getOmegaLambda0(const cosmoModel_t model);

extern double
cosmoModel_getOmegaMatter0(const cosmoModel_t model);

extern double
cosmoModel_getOmegaBaryon0(const cosmoModel_t model);

extern double
cosmoModel_getSmallH(const cosmoModel_t model);

extern double
cosmoModel_getSigma8(const cosmoModel_t model);

extern double
cosmoModel_getNs(const cosmoModel_t model);

extern double
cosmoModel_getTempCMB(const cosmoModel_t model);

extern void
cosmoModel_setOmegaRad0(cosmoModel_t model, const double omegaRad0);

extern void
cosmoModel_setOmegaLambda0(cosmoModel_t model, const double omegaLambda0);

extern void
cosmoModel_setOmegaMatter0(cosmoModel_t model, const double omegaMatter0);

extern void
cosmoModel_setOmegaBaryon0(cosmoModel_t model, const double omegaBaryon0);

extern void
cosmoModel_setSmallH(cosmoModel_t model, const double hubble);

extern void
cosmoModel_setSigma8(cosmoModel_t model, const double sigma8);

extern void
cosmoModel_setNs(cosmoModel_t model, const double ns);

extern void
cosmoModel_setTempCMB(cosmoModel_t model, const double tempCMB);

extern double
cosmoModel_calcAgeFromExpansion(cosmoModel_t model,
                                double       a,
                                double       *error);

extern double
cosmoModel_calcExpansionFromAge(cosmoModel_t model,
                                double       t,
                                double       *error);

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

extern double
cosmoModel_calcDlnGrowthDlna(cosmoModel_t model, double a, double *error);


#endif
