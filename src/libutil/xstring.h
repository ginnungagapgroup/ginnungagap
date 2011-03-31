// Copyright (C) 2010, 2011, Steffen Knollmann
// Released under the terms of the GNU General Public License version 3.
// This file is part of `ginnungagap'.

#ifndef XSTRING_H
#define XSTRING_H


/*--- Doxygen file description ------------------------------------------*/

/**
 * @file libutil/xstring.h
 * @ingroup libutilCore
 * @brief This file provides utility functions dealing with strings.
 */


/*--- Includes ----------------------------------------------------------*/
#include "util_config.h"
#include <stdio.h>


/*--- Prototypes of exported functions ----------------------------------*/

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


/**
 * \brief  This takes two strings and generates a new one that consists
 *         of the concatenation of the two.
 *
 * \param  *s1  The first string.
 * \param  *s2  The second string.
 *
 * \return  Returns a new string that is equivalent to ("%s%s",s1, s2).
 *          The user must free the string if now longer needed.
 */
extern char *
xstrmerge(const char *s1, const char *s2);


extern char *
xdirname(const char *path);

extern char *
xbasename(const char *path);

#endif
