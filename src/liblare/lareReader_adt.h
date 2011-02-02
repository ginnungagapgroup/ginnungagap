// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef LAREREADER_ADT_H
#define LAREREADER_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "lareConfig.h"
#include "lareReader.h"
#include "lare.h"
#include "lareIO.h"
#include "../libutil/parse_ini.h"


/*--- Internal structures -----------------------------------------------*/
struct lareReader_func_struct {
	void   (*del)(lareReader_t *reader);
	lare_t (*read)(lareReader_t reader);
};

typedef struct lareReader_func_struct *lareReader_func_t;


/*--- ADT implementation ------------------------------------------------*/
#define LAREREADER_T_CONTENT \
    lareIO_type_t type;      \
    lareReader_func_t func;

struct lareReader_struct {
	LAREREADER_T_CONTENT
};


#endif
