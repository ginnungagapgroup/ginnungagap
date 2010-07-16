// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BOV_ADT_H
#define BOV_ADT_H


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include "bov.h"
#include <stdbool.h>
#include <stdint.h>
#include "endian.h"


/*--- ADT implementation ------------------------------------------------*/
struct bov_struct {
	// Handling information
	char           *bovFileName;
	char           *bovFilePath;
	endian_t       machineEndianess;
	// Required BOV entries
	double         time;
	char           *data_file;
	uint32_t       data_size[3];
	bovFormat_t    data_format;
	char           *variable;
	endian_t       data_endian;
	bovCentering_t centering;
	double         brick_origin[3];
	double         brick_size[3];
	// Optional BOV entries
	int            byte_offset;
	bool           divide_brick;
	int            data_bricklets[3];
	int            data_components;
};


#endif
