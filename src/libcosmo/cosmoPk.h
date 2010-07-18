// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOPK_H
#define COSMOPK_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoModel.h"
#include "cosmoTF.h"
#include "../libutil/parse_ini.h"
#include <stdint.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct cosmoPk_struct *cosmoPk_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern cosmoPk_t
cosmoPk_newFromFile(const char *fname);

extern cosmoPk_t
cosmoPk_newFromModel(const cosmoModel_t model,
                     double             kmin,
                     double             kmax,
                     uint32_t           numPoints,
                     cosmoTF_t          transferFunctionType);

extern cosmoPk_t
cosmoPk_newFromIni(parse_ini_t ini, const char *sectionName);

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
//!     =    1/(8 \pi^3)
//!         \int_{k_{\mathrm{min}}}^{k_{\mathrm{max}}}
//!             k^(2n + 2) P(k) W_k^2(k)
//!         dk
extern double
cosmoPk_calcMomentFiltered(cosmoPk_t pk,
                           uint32_t moment,
                           double (*windowFunc
                                   )(double, void *),
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

extern void
cosmoPk_scale(cosmoPk_t pk, double factor);

extern double
cosmoPk_forceSigma8(cosmoPk_t pk,
                    double    sigma8,
                    double    kmin,
                    double    kmax,
                    double    *error);

extern void
cosmoPk_findKWindowForSigma8(cosmoPk_t pk, double *kmin, double *kmax);


#endif
