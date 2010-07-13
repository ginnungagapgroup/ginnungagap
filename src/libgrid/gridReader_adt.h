// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef GRIDREADER_ADT_H
#define GRIDREADER_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridIO.h"
#include "../libutil/parse_ini.h"


/*--- Internal structures -----------------------------------------------*/
struct gridReader_func_struct {
	void         (*del)(gridReader_t *gridReader);
};

typedef struct gridReader_func_struct *gridReader_func_t;


/*--- ADT implementation ------------------------------------------------*/
struct gridReader_struct {
	gridIO_type_t     type;
	gridReader_func_t func;
};


#endif
