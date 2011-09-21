// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMO_CONFIG_H
#define COSMO_CONFIG_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libcosmo/cosmo_config.h
 * @ingroup libcosmo
 * @brief This is main configuration header that all files in this
 *        library include.
 *
 * This header may be used to facilitate the re-use of the code in this
 * library in other projects.  Basically, the only dependencies outside
 * of the source directory should be pulled in from here (e.g. the
 * inclusion of the true config.h).  This is not quite true at the
 * moment, as various files do use external headers (mainly from @ref
 * libutil for error wrapping of standard functions or the ini parser).
 * The basic idea is that one could define one master define here which
 * prevents the other files from including anything outside of this
 * directory.  Of course one would loose functionality (like the ini
 * parser) while other things (like the error wrappers) need to be
 * defined to their original counterparts (e.g. using malloc instead of
 * xmalloc).
 */


/*--- Includes ----------------------------------------------------------*/
#include "../../config.h"
#include "../libutil/utilMath.h"


/*--- Defines for the library --------------------------------------------*/
#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif
#ifndef POW2
#  define POW2(x) ((x) * (x))
#endif
#ifndef POW3
#  define POW3(x) ((x) * (x) * (x))
#endif


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup libcosmo Library for cosmology functions
 * @brief This library provides functionality to handle cosmology
 *        related things.
 *
 * In essence, three things are provided by this library, the handling
 * of power spectra (\ref libcosmoPowerspectrum), the encapsulation of
 * the composition of the universe and the calculation of expansion
 * factors et al. (\ref libcosmoModel)
 */

#endif
