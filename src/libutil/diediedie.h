// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DIEDIEDIE_H
#define DIEDIEDIE_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>


/*--- Exported defines --------------------------------------------------*/
#  define diediedie(errCode)                              \
	{                                                     \
		fprintf(stderr,                                   \
		        "FATAL:  Deathtrap in %s() at %s:%i\n"    \
		        "Terminating with error code: %i\n\n",    \
		        __func__, __FILE__, __LINE__, (errCode)); \
		exit(errCode);                                    \
	}


#endif
