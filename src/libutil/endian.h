// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef ENDIAN_H
#define ENDIAN_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/endian.h
 * @ingroup libutilMisc
 * @brief  This file provides functionality to deal with endianess.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdbool.h>


/*--- Typedefs ----------------------------------------------------------*/

/**
 * @brief The possible endianesses.
 */
typedef enum {
	/** That would be little endian. */
	ENDIAN_LITTLE,
	/** That would be big endian. */
	ENDIAN_BIG
} endian_t;


/*--- Prototypes of exported functions ----------------------------------*/

/**
 * @brief  Checks whether the system is little endian.
 * @return  Returns true if the system is little endian, false
 *          otherwise.
 */
extern bool
endian_systemIsLittle(void);


/**
 * @brief  Checks whether the system is big endian.
 * @return  Returns true if the system is big endian, false
 *          otherwise.
 */
extern bool
endian_systemIsBig(void);


/**
 * @brief  Discovers the endianess of the system.
 * @return  Returns the system endianess.
 */
extern endian_t
endian_getSystemEndianess(void);


/**
 * @brief  Checks whether a file is little endian using block delimiter.
 * @return  Returns true if the file is written in little endian, false
 *          otherwise.
 */
extern bool
endian_fileIsLittleByBlock(const char *fname);


/**
 * @brief  Checks whether a file is big endian using block delimiter.
 * @return  Returns true if the file is written in big endian, false
 *          otherwise.
 */
extern bool
endian_fileIsBigByBlock(const char *fname);


/**
 * @brief  Checks for the endianess of a file using block delimiter.
 * @return  Returns the endianess of the file.
 */
extern endian_t
endian_getFileEndianessByBlock(const char *fname);


#endif
