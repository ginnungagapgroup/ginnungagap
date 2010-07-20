// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRAFICS_TESTS_H
#define GRAFICS_TESTS_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Prototypes of exported functions ----------------------------------*/
extern bool
grafic_new_test(void);

extern bool
grafic_newFromFile_test(void);

extern bool
grafic_del_test(void);

extern bool
grafic_getFileName_test(void);

extern bool
grafic_getSize_test(void);

extern bool
grafic_getDx_test(void);

extern bool
grafic_getXoff_test(void);

extern bool
grafic_getAstart_test(void);

extern bool
grafic_getOmegam_test(void);

extern bool
grafic_getOmegav_test(void);

extern bool
grafic_getH0_test(void);

extern bool
grafic_getIseed_test(void);

extern bool
grafic_setFileName_test(void);

extern bool
grafic_setSize_test(void);

extern bool
grafic_setDx_test(void);

extern bool
grafic_setXoff_test(void);

extern bool
grafic_setAstart_test(void);

extern bool
grafic_setOmegam_test(void);

extern bool
grafic_setOmegav_test(void);

extern bool
grafic_setH0_test(void);

extern bool
grafic_setIseed_test(void);

extern bool
grafic_isWhiteNoise_test(void);

extern bool
grafic_makeEmptyFile_test(void);

extern bool
grafic_read_test(void);

extern bool
grafic_write_test(void);

extern bool
grafic_readWindowed_test(void);

extern bool
grafic_writeWindowed_test(void);


#endif
