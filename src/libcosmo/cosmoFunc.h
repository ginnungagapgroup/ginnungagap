// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMOFUNC_H
#define COSMOFUNC_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"


/*--- Prototypes of exported functions ----------------------------------*/

/// This is returns factor for all x.
extern double
cosmoFunc_const(double x, void *value);

extern double
cosmoFunc_tophat(double k, void *lengthScale);

extern double
cosmoFunc_tophatSqr(double k, void *lengthScale);

extern double
cosmoFunc_dtophat(double x, void *lengthScale);


/// This is x^exponent.
extern double
cosmoFunc_xPowerY(double x, void *exponent);


/// This is f1(x) * f2(x) * f3(x)
extern double
cosmoFunc_product3(double x, void *params);


typedef struct cosmoFunc_product3_struct {
	double (*f1)(double, void *);
	void *paramF1;
	double (*f2)(double, void *);
	void *paramF2;
	double (*f3)(double, void *);
	void *paramF3;
} cosmoFunc_product3_struct_t;

typedef cosmoFunc_product3_struct_t *cosmoFunc_product3_t;

/// This is
/// \frac{dt}{da} = (
///                    \Omega_{r,0} (1/a^2 - 1)
///                   +\Omega_{m,0} (1/a -1.0)
///                   +\Omega_{\Lambda,0} (a^2 - 1)
///                   +1
///                 )^(-1/2)
extern double
cosmoFunc_dtda(double a, void *param);


/// This is (\frac{(dt}{da})^3
extern double
cosmoFunc_dtdaCube(double a, void *param);


typedef struct cosmoFunc_dtda_struct {
	double omegaRad0;
	double omegaMatter0;
	double omegaLambda0;
} cosmoFunc_dtda_struct_t;

typedef cosmoFunc_dtda_struct_t *cosmoFunc_dtda_t;


#endif
