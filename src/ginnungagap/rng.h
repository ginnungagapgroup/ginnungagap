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
rng_new(parse_ini_t ini);

extern void
rng_del(rng_t *rng);


#endif
