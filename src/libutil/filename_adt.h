// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef FILENAME_ADT_H
#define FILENAME_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/filename_adt.h
 * @ingroup  libutilMiscFilename
 * @brief  Implements the main structure of the filename object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main filename structure. */
struct filename_struct {
	/** @brief  The path of the file. */
	const char *path;
	/** @brief  The prefix of the file. */
	const char *prefix;
	/** @brief  The qualifier of the file. */
	const char *qualifier;
	/** @brief  The suffix of the file. */
	const char *suffix;
	/** @brief  The full name of the file. */
	const char *fullName;
};


#endif
