// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOPK_H
#define COSMOPK_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include <stdint.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct cosmoPk_struct *cosmoPk_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern cosmoPk_t
cosmoPk_newFromFile(const char *fname);

extern cosmoPk_t
cosmoPk_newFromArrays(uint32_t     numPoints,
                      const double *k,
                      const double *P,
                      double       slopeBeforeKmin,
                      double       slopeBeyondKmax);

extern void
cosmoPk_del(cosmoPk_t *pk);

extern void
cosmoPk_dumpToFile(cosmoPk_t pk, const char *fname, uint32_t numSubSample);

extern double
cosmoPk_eval(cosmoPk_t pk, double k);

extern double
cosmoPk_evalGSL(double k, void *param);


//! This implements the calculation of the moments of the power spectrum
//! filtered with a window function.  Solved is the following integral
//! (cf. Peacock 2007, 8th edition, eq. (16.22), p. 500):
//!
//! \sigma_n^2 / V
//!     =    1/(2 \pi^2)
//!         \int_{k_{\mathrm{min}}}^{k_{\mathrm{max}}}
//!             k^(2n + 2) P(k) W_k^2(k)
//!         dk
extern double
cosmoPk_calcMomentFiltered(cosmoPk_t pk,
                           uint32_t moment,
                           double (*windowFunc)(double, void *),
                           void *paramWindowFunc,
                           double kmin,
                           double kmax,
                           double *error);


//! Calculates \sigma_8 and NOT(!!!) \sigma^2_8.  Uses the moments.
extern double
cosmoPk_calcSigma8(cosmoPk_t pk,
                   double    kmin,
                   double    kmax,
                   double    *error);


#endif
