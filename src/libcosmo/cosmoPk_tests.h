// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOPK_TESTS_H
#define COSMOPK_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
cosmoPk_newFromFile_test(void);

extern bool
cosmoPk_newFromModel_test(void);

extern bool
cosmoPk_newFromArrays_test(void);

extern bool
cosmoPk_del_test(void);

extern bool
cosmoPk_eval_test(void);

extern bool
cosmoPk_calcMomentFiltered_test(void);

extern bool
cosmoPk_calcSigma8_test(void);

extern bool
cosmoPk_scale_test(void);

extern bool
cosmoPk_forceSigma8_test(void);

extern bool
cosmoPk_forceAmplitude_test(void);


#endif
