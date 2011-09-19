// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRAFIC2GADGET_H
#define GRAFIC2GADGET_H


/*--- Includes ----------------------------------------------------------*/
#include "grafic2gadgetConfig.h"
#include <stdbool.h>


/*--- ADT handle --------------------------------------------------------*/
typedef struct grafic2gadget_struct *grafic2gadget_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern grafic2gadget_t
grafic2gadget_new(const char *graficFileNameVx,
                  const char *graficFileNameVy,
                  const char *graficFileNameVz,
                  const char *outputFileStem,
                  int        numOutFiles,
                  bool       force,
                  bool       useLong);

extern void
grafic2gadget_run(grafic2gadget_t g2g);

extern void
grafic2gadget_del(grafic2gadget_t *g2g);


#endif
