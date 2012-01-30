// Copyright (C) 2010, 2011, 2012 Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOTF_H
#define COSMOTF_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libcosmo/cosmoTF.h
 * @ingroup  libcosmoTF
 * @brief  Provides the interface cosmological transfer functions.
 */


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "../libutil/parse_ini.h"
#include <stdint.h>


/*--- Exported types ----------------------------------------------------*/

/** @brief  The different transfer functions. */
typedef enum {
	/** @brief  For Eisenstein & Hu 1998. */
	COSMOTF_TYPE_EISENSTEINHU1998,
	/** @brief  A scale free transfer function. */
	COSMOTF_TYPE_SCALEFREE,
	/** @brief  For Klypin 2000. */
	COSMOTF_TYPE_ANATOLY2000
} cosmoTF_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern cosmoTF_t
cosmoTF_getTypeFromIni(parse_ini_t ini, const char *sectionName);

extern void
cosmoTF_eisensteinHu1998(double   omegaMatter0,
                         double   omegaBaryon0,
                         double   hubble,
                         double   tempCMB,
                         uint32_t numPoints,
                         double   *k,
                         double   *T);

extern void
cosmoTF_scaleFree(uint32_t numPoints,
                  double   *k,
                  double   *T);


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libcosmoTF Transfer Functions
 * @ingroup libcosmo
 * @brief Provides transfer functions.
 */


#endif
