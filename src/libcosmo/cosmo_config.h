// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef COSMO_CONFIG_H
#define COSMO_CONFIG_H


#include "../../config.h"


#ifndef diediedie
#  define diediedie(errCode)                              \
	{                                                     \
		fprintf(stderr,                                   \
		        "FATAL:  Deathtrap in %s() at %s:%i\n"    \
		        "Terminating with error code: %i\n\n",    \
		        __func__, __FILE__, __LINE__, (errCode)); \
		exit(errCode);                                    \
	}
#endif
#ifndef PI
#  define PI 3.14159265359
#endif
#ifndef TWOPI
#  define TWOPI 6.28318530718
#endif
#ifndef POW2
#  define POW2(x) ((x) * (x))
#endif
#ifndef POW3
#  define POW3(x) ((x) * (x) * (x))
#endif


#endif
