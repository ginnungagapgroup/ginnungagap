// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoTF_tests.h"
#include "cosmoPk_tests.h"
#include "cosmoModel_tests.h"
#include <stdio.h>
#include <stdlib.h>


/*--- Macros ------------------------------------------------------------*/
#define RUNTEST(a, hasFailed)  \
    if (!(a())) {              \
		printf("!! FAILED !!\n");    \
		hasFailed = true;      \
	} else {                   \
		printf("passed\n");    \
		if (!hasFailed)        \
			hasFailed = false; \
	}


/*--- M A I N -----------------------------------------------------------*/
int
main(void)
{
	bool hasFailed = false;

	printf("Running tests for cosmoTF:\n");
	RUNTEST(cosmoTF_eisensteinHu1998_test, hasFailed);

	printf("\nRunning tests for cosmoPk:\n");
	RUNTEST(cosmoPk_newFromFile_test, hasFailed);
	RUNTEST(cosmoPk_newFromModel_test, hasFailed);
	RUNTEST(cosmoPk_newFromArrays_test, hasFailed);
	RUNTEST(cosmoPk_del_test, hasFailed);
	RUNTEST(cosmoPk_eval_test, hasFailed);
	RUNTEST(cosmoPk_calcMomentFiltered_test, hasFailed);
	RUNTEST(cosmoPk_calcSigma8_test, hasFailed);
	RUNTEST(cosmoPk_scale_test, hasFailed);
	RUNTEST(cosmoPk_forceSigma8_test, hasFailed);

	printf("\nRunning tests for cosmoModel:\n");
	RUNTEST(cosmoModel_newFromFile_test, hasFailed);
	RUNTEST(cosmoModel_del_test, hasFailed);
	RUNTEST(cosmoModel_getOmegaRad0_test, hasFailed);
	RUNTEST(cosmoModel_getOmegaLambda0_test, hasFailed);
	RUNTEST(cosmoModel_getOmegaMatter0_test, hasFailed);
	RUNTEST(cosmoModel_getOmegaBaryon0_test, hasFailed);
	RUNTEST(cosmoModel_getSmallH_test, hasFailed);
	RUNTEST(cosmoModel_getSigma8_test, hasFailed);
	RUNTEST(cosmoModel_getNs_test, hasFailed);
	RUNTEST(cosmoModel_calcAgeFromExpansion_test, hasFailed);
	RUNTEST(cosmoModel_calcExpansionFromAge_test, hasFailed);
	RUNTEST(cosmoModel_calcADot_test, hasFailed);
	RUNTEST(cosmoModel_calcOmegas_test, hasFailed);
	RUNTEST(cosmoModel_calcGrowth_test, hasFailed);

	if (hasFailed) {
		fprintf(stderr, "\nSome tests failed!\n\n");
		return EXIT_FAILURE;
	}
	printf("\nAll tests passed successfully!\n\n");
	return EXIT_SUCCESS;
}
