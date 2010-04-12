// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"
#include "cosmoFunc.h"
#include <math.h>


/*--- Implementations of exported functios ------------------------------*/
extern double
cosmoFunc_const(double x, void *value)
{
	return *((double *)value);
}

extern double
cosmoFunc_tophat(double k, void *lengthScale)
{
	double x = *((double *)lengthScale) * k;
	return 3. * (sin(x) - x * cos(x)) / (x * x * x);
}

extern double
cosmoFunc_tophatSqr(double k, void *lengthScale)
{
	double tmp =  cosmoFunc_tophat(k, lengthScale);
	return tmp*tmp;
}

extern double
cosmoFunc_dtophat(double k, void *lengthScale)
{
	double x = *((double *)lengthScale) * k;
	double sx, x2inv;
	sx    = sin(x);
	x2inv = 1. / POW2(x);

	return -9. * POW2(x2inv) * (sx - x * cos(x)) + 3. * x2inv * sx;
}

extern double
cosmoFunc_xPowerY(double x, void *exponent)
{
	return pow(x, *((double *)exponent));
}

extern double
cosmoFunc_product3(double x, void *params)
{
	cosmoFunc_product3_t p = (cosmoFunc_product3_t)params;
	return (p->f1(x, p->paramF1)) * (p->f2(x, p->paramF2))
	       * (p->f3(x, p->paramF3));
}

extern double
cosmoFunc_dtda(double a, void *param)
{
	cosmoFunc_dtda_t p    = (cosmoFunc_dtda_t)param;
	double           aInv = 1. / a;
	return 1. / sqrt(p->omegaRad0 * (aInv * aInv - 1.0)
	                 + p->omegaMatter0 * (aInv - 1.0)
	                 + p->omegaLambda0 * (a * a - 1.0)
	                 + 1.0);
}

extern double
cosmoFunc_dtdaCube(double a, void *param)
{
	double tmp = cosmoFunc_dtda(a, param);
	return tmp * tmp * tmp;
}
