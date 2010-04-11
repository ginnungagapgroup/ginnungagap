// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef RNG_H
#define RNG_H


/*--- Includes ----------------------------------------------------------*/
#include "../libutil/parse_ini.h"


/*--- ADT handle --------------------------------------------------------*/
typedef struct rng_struct *rng_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern rng_t
rng_new(int generatorType, int numStreamsTotal, int randomSeed);

extern rng_t
rng_newFromIni(parse_ini_t ini, const char *sectionName);

extern void
rng_del(rng_t *rng);

extern double
rng_getGauss(const rng_t  rng,
             const int    streamNumber,
             const double mean,
             const double sigma);

extern double
rng_getGaussUnit(const rng_t rng, const int streamNumber);

#endif
