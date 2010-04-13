// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.


/*--- Includes ----------------------------------------------------------*/
#include "gridConfig.h"
#include "gridVarType.h"
#include <assert.h>
#include "../libutil/xmem.h"
#include "../libutil/diediedie.h"


/*--- Local defines -----------------------------------------------------*/


/*--- Prototypes of local functions -------------------------------------*/


/*--- Implementations of exported functios ------------------------------*/
extern size_t
gridVarType_sizeof(gridVarType_t type)
{
	size_t size;

	switch (type) {
	case GRIDVARTYPE_INT:
		size = sizeof(int);
		break;
	case GRIDVARTYPE_DOUBLE:
		size = sizeof(double);
		break;
	case GRIDVARTYPE_FPV:
		size = sizeof(fpv_t);
		break;
	default:
		diediedie(EXIT_FAILURE);
	}

	return size;
}

extern bool
gridVarType_isFloating(gridVarType_t type)
{
	switch (type) {
	case GRIDVARTYPE_DOUBLE:
	case GRIDVARTYPE_FPV:
		return true;
	}

	return false;
}

extern bool
gridVarType_isInteger(gridVarType_t type)
{
	switch (type) {
	case GRIDVARTYPE_INT:
		return true;
	}

	return false;
}

extern bool
gridVarType_isNativeFloat(gridVarType_t type)
{
	switch (type) {
#ifndef ENABLE_DOUBLE
	case GRIDVARTYPE_FPV:
		return true;
#endif
	}

	return false;
}

extern bool
gridVarType_isNativeDouble(gridVarType_t type)
{
	switch (type) {
	case GRIDVARTYPE_DOUBLE:
#ifdef ENABLE_DOUBLE
	case GRIDVARTYPE_FPV:
#endif
		return true;
	}

	return false;
}

/*--- Implementations of local functions --------------------------------*/
