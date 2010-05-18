// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.
//
// This file is an interface to the code provided by Daniel J.
// Eisenstein and Wayne Hu.  See comments in the according source file
// this is the header to, for further clarification.

#ifndef COSMOTFEISENSTEINHU_H
#define COSMOTFEISENSTEINHU_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"


/*--- Prototypes of exported functions ----------------------------------*/
extern void
TFfit_hmpc(double omega0,
           double f_baryon,
           double hubble,
           double Tcmb,
           int    numk,
           double *k,
           double *tf_full,
           double *tf_baryon,
           double *tf_cdm);

#endif
