// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef BYTESWAP_H
#define BYTESWAP_H


#include "util_config.h"
#include <stdlib.h>


/**
 * \brief  Performs a byteswapping.
 *
 * \param  *p  The pointer to the variable that should be byteswapped.
 * \param  s   The number of bytes of the variable.
 *
 * \return  Returns nothing.
 */
extern void
byteswap(void *p, size_t s);


#endif
