// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "dataConfig.h"
#include "dataVarType.h"
#include <assert.h>
#include <stdint.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern size_t
dataVarType_sizeof(dataVarType_t type)
{
	size_t size;

	switch (type) {
	case DATAVARTYPE_INT8:
		size = sizeof(int8_t);
		break;
	case DATAVARTYPE_INT:
		size = sizeof(int);
		break;
	case DATAVARTYPE_INT32:
		size = sizeof(int32_t);
		break;
	case DATAVARTYPE_INT64:
		size = sizeof(int64_t);
		break;
	case DATAVARTYPE_FLOAT:
		size = sizeof(float);
		break;
	case DATAVARTYPE_DOUBLE:
		size = sizeof(double);
		break;
	case DATAVARTYPE_FPV:
		size = sizeof(fpv_t);
		break;
	default:
		diediedie(EXIT_FAILURE);
	}

	return size;
}

extern bool
dataVarType_isFloating(dataVarType_t type)
{
	switch (type) {
	case DATAVARTYPE_DOUBLE:
	case DATAVARTYPE_FLOAT:
	case DATAVARTYPE_FPV:
		return true;

	default:
		break;
	}

	return false;
}

extern bool
dataVarType_isInteger(dataVarType_t type)
{
	switch (type) {
	case DATAVARTYPE_INT8:
	case DATAVARTYPE_INT:
	case DATAVARTYPE_INT32:
	case DATAVARTYPE_INT64:
		return true;

	default:
		break;
	}

	return false;
}

extern bool
dataVarType_isNativeFloat(dataVarType_t type)
{
	switch (type) {
	case DATAVARTYPE_FLOAT:
#ifndef ENABLE_DOUBLE
	case DATAVARTYPE_FPV:
#endif
		return true;
	default:
		break;
	}

	return false;
}

extern bool
dataVarType_isNativeDouble(dataVarType_t type)
{
	switch (type) {
	case DATAVARTYPE_DOUBLE:
#ifdef ENABLE_DOUBLE
	case DATAVARTYPE_FPV:
#endif
		return true;

	default:
		break;
	}

	return false;
}

/*--- Implementations of local functions --------------------------------*/
