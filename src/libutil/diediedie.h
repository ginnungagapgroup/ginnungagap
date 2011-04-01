// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef DIEDIEDIE_H
#define DIEDIEDIE_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/diediedie.h
 * @ingroup libutilMisc
 * @brief  Provides a macro to stop the program with the indication
 *         where it failed.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>


/*--- Exported defines --------------------------------------------------*/

/**
 * @brief  Terminates the program with an error code.
 *
 * The macro will print an error message to @a stderr and indicate the
 * function, file and line that caused the error.  This is to be used as
 * a worst case program termination.  Note that <b>no</b> clean-up is
 * performed, if specific things are required they should have been
 * registered with <b>at_exit</b>(3).
 */
#define diediedie(errCode)                                \
	{                                                     \
		fprintf(stderr,                                   \
		        "FATAL:  Deathtrap in %s() at %s:%i\n"    \
		        "Terminating with error code: %i\n\n",    \
		        __func__, __FILE__, __LINE__, (errCode)); \
		exit(errCode);                                    \
	}


#endif
