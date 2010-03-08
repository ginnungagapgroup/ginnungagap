// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarTypes.h"
#include <stdlib.h>
#include <stdint.h>


/*--- Implementations of exported functios ------------------------------*/
extern size_t
gridVarTypes_getSizePerElement(gridVarTypes_t type)
{
	size_t sizePerElement;

	switch (type) {
	case GRIDVARTYPES_FPV:
		sizePerElement = sizeof(fpv_t);
		break;
	case GRIDVARTYPES_FPVVEC:
		sizePerElement = sizeof(fpv_t) * NDIM;
		break;
	case GRIDVARTYPES_INT32:
		sizePerElement = sizeof(int32_t);
		break;
	case GRIDVARTYPES_INT32VEC:
		sizePerElement = sizeof(int32_t) * NDIM;
		break;
	default:
		exit(EXIT_FAILURE);
	}

	return sizePerElement;
}
