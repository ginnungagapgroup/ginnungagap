// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BOILER_H
#define BOILER_H


/*--- Includes ----------------------------------------------------------*/
#include "CONF_HEADER"


/*--- ADT handle --------------------------------------------------------*/
typedef struct boiler_struct *boiler_t;


/*--- Prototypes of exported functions ----------------------------------*/
extern boiler_t
boiler_new(void);

extern void
boiler_del(boiler_t *boiler);


#endif
