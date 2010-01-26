// Copyright (C) 2010, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef XSTRING_H
#define XSTRING_H


#include "util_config.h"
#include <stdio.h>


/**
 * \brief  Clone of the GNU strdup() function
 *
 * This functions returns a pointer to a freshly allocated memory
 * containing a copy of the provided string.  Memory for the new string
 * is obtained with malloc and should be freed accordingly when not
 * needed anymore.
 *
 * \param  *s  The string to duplicate.
 *
 * \return  A pointer to the duplicated string.
 */
extern char *
xstrdup(const char *s);


/**
 * \brief  Clone of the GNU getline() function.
 *
 * \param  **line  Pointer to the external variable holding the line
 *                 buffer.  This buffer will be increased if needed.
 * \param  *n      The number of bytes of the buffer.
 * \param  *f      The file to read from.
 *
 * \return  Returns the number of characters read, or if an error
 *          occured, -1.
 */
extern size_t
xgetline(char **line, size_t *n, FILE *f);


#endif
