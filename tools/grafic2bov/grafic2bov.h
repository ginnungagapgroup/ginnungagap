// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRAFIC2BOV_H
#define GRAFIC2BOV_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file grafic2bov/grafic2bov.h
 * @ingroup  toolsGrafic2Bov
 * @brief  Provides the interface to the grafic2bov tool.
 */


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


/*--- Doxygen group definitions -----------------------------------------*/

/**
 * @defgroup toolsGrafic2Bov grafic2bov
 * @ingroup  tools
 * @brief  Provides the grafic2bov tool.
 */


#endif
