// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BOV_ADT_H
#define BOV_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/bov_adt.h
 * @ingroup libutilFilesBov
 * @brief  This file implements the main BOV structure.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "bov.h"
#include <stdbool.h>
#include <stdint.h>
#include "endian.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main BOV structure holding all required information.  */
struct bov_struct {
	//
	// Handling information
	//
	/** @brief The name of the .bov file. */
	char     *bovFileName;
	/** @brief The path to the .bov file. */
	char     *bovFilePath;
	/** @brief The endianess of the machine. */
	endian_t machineEndianess;
	//
	// Required BOV entries
	//
	/** @brief The TIME value. */
	double         time;
	/** @brief The DATA_FILE value. */
	char           *data_file;
	/** @brief The DATA_SIZE value. */
	uint32_t       data_size[3];
	/** @brief The DATA_FORMAT value. */
	bovFormat_t    data_format;
	/** @brief The VARIABLE value. */
	char           *variable;
	/** @brief The DATA_ENDIAN value. */
	endian_t       data_endian;
	/** @brief The CENTERING value. */
	bovCentering_t centering;
	/** @brief The BRICK_ORIGIN value. */
	double         brick_origin[3];
	/** @brief The BRICK_SIZE value. */
	double         brick_size[3];
	//
	// Optional BOV entries
	//
	/** @brief The BYTE_OFFSET value. */
	int  byte_offset;
	/** @brief The DIVIDE_BRICK value. */
	bool divide_brick;
	/** @brief The DATA_BRICKLETS value. */
	int  data_bricklets[3];
	/** @brief The DATA_COMPONENTS value. */
	int  data_components;
};


#endif
