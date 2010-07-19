// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOMODEL_TESTS_H
#define COSMOMODEL_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
cosmoModel_newFromFile_test(void);

extern bool
cosmoModel_del_test(void);

extern bool
cosmoModel_getOmegaRad0_test(void);

extern bool
cosmoModel_getOmegaLambda0_test(void);

extern bool
cosmoModel_getOmegaMatter0_test(void);

extern bool
cosmoModel_getOmegaBaryon0_test(void);

extern bool
cosmoModel_getSmallH_test(void);

extern bool
cosmoModel_getSigma8_test(void);

extern bool
cosmoModel_getNs_test(void);

extern bool
cosmoModel_calcAgeFromExpansion_test(void);

extern bool
cosmoModel_calcExpansionFromAge_test(void);

extern bool
cosmoModel_calcADot_test(void);

extern bool
cosmoModel_calcOmegas_test(void);

extern bool
cosmoModel_calcGrowth_test(void);

extern bool
cosmoModel_calcDlnGrowthDlna_test(void);

#endif
