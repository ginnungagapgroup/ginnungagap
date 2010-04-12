// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


#ifndef COSMOMODEL_ADT_H
#define COSMOMODEL_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "cosmo_config.h"


/*--- Implemention of the ADT structure ---------------------------------*/
struct cosmoModel_struct {
	double omegaRad0;
	double omegaLambda0;
	double omegaMatter0;
	double omegaBaryon0;
	double hubble;
	double sigma8;
	double ns;
};


#endif
