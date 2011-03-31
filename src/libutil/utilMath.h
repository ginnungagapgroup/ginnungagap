// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef UTILMATH_H
#define UTILMATH_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/utilMath.h
 * @ingroup libutilMisc
 * @brief This file defines useful mathematical constants.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- Exported defines --------------------------------------------------*/

//! This is e
#define M_E 2.7182818284590452354
//! This is log_2 e
#define M_LOG2E 1.4426950408889634074
//! This is log_10 e
#define M_LOG10E 0.43429448190325182765
//! This is log_e 2
#define M_LN2 0.69314718055994530942
//! This is log_e 10
#define M_LN10 2.30258509299404568402
//! This is pi
#define M_PI 3.14159265358979323846
//! This is pi/2
#define M_PI_2 1.57079632679489661923
//! This is pi/4
#define M_PI_4 0.78539816339744830962
//! This is 1/pi
#define M_1_PI 0.31830988618379067154
//! This is 2/pi
#define M_2_PI 0.63661977236758134308
//! This is 2/sqrt(pi)
#define M_2_SQRTPI 1.12837916709551257390
//! This is sqrt(2)
#define M_SQRT2 1.41421356237309504880
//! This is 1/sqrt(2)
#define M_SQRT1_2 0.70710678118654752440

//! This is e in long double
#define M_El 2.7182818284590452353602874713526625L
//! This is log_2 e in long double
#define M_LOG2El 1.4426950408889634073599246810018921L
//! This is log_10 e in long double
#define M_LOG10El 0.4342944819032518276511289189166051L
//! This is log_e 2 in long double
#define M_LN2l 0.6931471805599453094172321214581766L
//! This is log_e 10 in long double
#define M_LN10l 2.3025850929940456840179914546843642L
//! This is pi in long double
#define M_PIl 3.1415926535897932384626433832795029L
//! This is pi/2 in long double
#define M_PI_2l 1.5707963267948966192313216916397514L
//! This is pi/4 in long double
#define M_PI_4l 0.7853981633974483096156608458198757L
//! This is 1/pi in long double
#define M_1_PIl 0.3183098861837906715377675267450287L
//! This is 2/pi in long double
#define M_2_PIl 0.6366197723675813430755350534900574L
//! This is 2/sqrt(pi) in long double
#define M_2_SQRTPIl 1.1283791670955125738961589031215452L
//! This is sqrt(2) in long double
#define M_SQRT2l 1.4142135623730950488016887242096981L
//! This is 1/sqrt(2) in long double
#define M_SQRT1_2l 0.7071067811865475244008443621048490L


/*--- Exported Macros ---------------------------------------------------*/
#define POW2(a) ((a) * (a))
#define POW3(a) ((a) * (a) * (a))
#define POW4(a) ((a) * (a) * (a) * (a))


/*--- Prototypes of exported functions ----------------------------------*/


#endif
