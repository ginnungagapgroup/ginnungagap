// Copyright (C) 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `voelva'.

#ifndef STAI_ADT_H
#define STAI_ADT_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file stai_adt.h
 * @ingroup libutilMiscStai
 * @brief  Provides the main structure of the stai object.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"


/*--- ADT implementation ------------------------------------------------*/

/** @brief  The main structure of the stai object.  */
struct stai_struct {
	/** @brief  The base pointer to the start of the array. */
	void         *base;
	/** @brief  The size of one element in the array in bytes. */
	unsigned int sizeOfElementInBytes;
	/** @brief  The stride to the next element in bytes. */
	unsigned int strideInBytes;
};


#endif
