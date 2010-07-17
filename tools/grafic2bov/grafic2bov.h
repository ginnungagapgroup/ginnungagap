// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRAFIC2BOV_H
#define GRAFIC2BOV_H


/*--- Includes ----------------------------------------------------------*/
#include "grafic2bovConfig.h"
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct grafic2bov_struct *grafic2bov_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern grafic2bov_t
grafic2bov_new(const char *graficFileName,
               const char *outputFileStem,
               bool       force);

extern void
grafic2bov_run(grafic2bov_t g2b);

extern void
grafic2bov_del(grafic2bov_t *g2b);


#endif
