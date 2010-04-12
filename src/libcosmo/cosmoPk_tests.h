// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.

#ifndef COSMOPK_TESTS_H
#define COSMOPK_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
cosmoPk_newFromFile_test(void);

extern bool
cosmoPk_newFromArrays_test(void);

extern bool
cosmoPk_del_test(void);

extern bool
cosmoPk_eval_test(void);

extern bool
cosmoPk_calcMomentFiltered_test(void);


#endif
